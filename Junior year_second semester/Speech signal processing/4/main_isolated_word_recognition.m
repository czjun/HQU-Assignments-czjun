% 主程序：孤立词语音识别系统

% --- 初始化 ---
clear; clc; close all;

% --- 参数设置 ---
% 语音库路径设置为data文件夹
databasePath = 'data/'; 
% 词汇列表 - 更新为数字0-9
vocabulary = {'zero', 'one', 'two', 'three', 'four', 'five', 'six', 'seven', 'eight', 'nine'}; 
numWords = length(vocabulary);
numSamplesPerWord = 10; % 每个词的样本数量

% 端点检测参数
frameLength = 25; % 帧长 (ms)
frameShift = 10; % 帧移 (ms)
energyThreshold = 0.1; % 能量阈值，需要根据实际信号调整
zcrThreshold = 0.2; % 过零率阈值，需要根据实际信号调整

% MFCC参数
mfccDimension = 20; % MFCC特征维度

% --- 2. 端点检测和特征提取 ---
disp('--- 2. 端点检测和特征提取 ---');
allFeatures = cell(numWords, numSamplesPerWord); % 用于存储所有样本的MFCC特征
allEndpointInfo = cell(numWords, numSamplesPerWord); % 用于存储所有样本的端点信息

for i = 1:numWords
    wordName = vocabulary{i};
    fprintf('正在处理词汇: %s\n', wordName);
    wordDir = [databasePath, wordName];
    wavFiles = dir(fullfile(wordDir, '*.wav'));
    
    % 确保不超过指定的样本数量
    numSamples = min(length(wavFiles), numSamplesPerWord);
    
    for j = 1:numSamples
        % 使用实际文件名
        filePath = fullfile(wordDir, wavFiles(j).name);
        fprintf('  处理文件: %s\n', filePath);
        
        if ~exist(filePath, 'file')
            warning('文件 %s 不存在，跳过。', filePath);
            allFeatures{i,j} = [];
            allEndpointInfo{i,j} = [];
            continue;
        end
        
        % 读取语音文件
        [y, fs] = audioread(filePath);
        
        % 确保y是列向量
        if size(y, 2) > 1
            y = mean(y, 2); % 如果是多通道，取平均转为单通道
        end
        if isrow(y)
            y = y';
        end
        
        % 2.1 端点检测
        [startIndex, endIndex] = endpointDetection(y, fs, frameLength, frameShift, energyThreshold, zcrThreshold); 
        allEndpointInfo{i,j} = struct('start', startIndex, 'end', endIndex);
        y_segmented = y(startIndex:endIndex); % 截取有效语音段
        
        % 2.2 MFCC 特征提取 (使用 Audio Toolbox 的 mfcc 函数)
        coeffs = mfcc(y_segmented, fs);
        allFeatures{i,j} = coeffs;
        
        fprintf('    提取特征维度: %d x %d\n', size(allFeatures{i,j},1), size(allFeatures{i,j},2));
    end
end
disp('特征提取完成。');

% --- 3. 构建参考模板 ---
disp('--- 3. 构建参考模板 ---');
referenceTemplates = cell(1, numWords);
for i = 1:numWords
    % 使用每个词的前5个样本的平均特征作为参考模板
    validSamples = 0;
    sumFeatures = [];
    
    % 获取有效样本数量
    validSampleCount = 0;
    for j = 1:min(5, numSamplesPerWord) % 尝试使用前5个样本
        if ~isempty(allFeatures{i,j})
            validSampleCount = validSampleCount + 1;
        end
    end
    
    if validSampleCount > 0
        % 使用第一个有效样本作为参考模板
        for j = 1:min(5, numSamplesPerWord)
            if ~isempty(allFeatures{i,j})
                referenceTemplates{i} = allFeatures{i,j};
                fprintf('词汇 "%s" 的参考模板已创建 (使用第%d个样本)。\n', vocabulary{i}, j);
                break;
            end
        end
    else
        warning('词汇 "%s" 没有可用的特征样本来创建参考模板。', vocabulary{i});
        referenceTemplates{i} = [];
    end
end
disp('参考模板构建完成。');

% --- 5. 性能评估 - 使用留一交叉验证 ---
disp('--- 5. 性能评估 (留一交叉验证) ---');
correctCount = 0;
totalCount = 0;

confusionMatrix = zeros(numWords, numWords); % 混淆矩阵

for i = 1:numWords % 遍历所有词汇
    wordDir = [databasePath, vocabulary{i}];
    wavFiles = dir(fullfile(wordDir, '*.wav'));
    numSamples = min(length(wavFiles), numSamplesPerWord);
    
    for j = 1:numSamples % 遍历该词汇下的所有样本
        if isempty(allFeatures{i,j})
            continue; % 跳过没有特征的样本
        end
        
        fprintf('评估: 词汇 "%s", 样本 %d\n', vocabulary{i}, j);
        
        % 当前样本作为测试样本
        testFeatures_eval = allFeatures{i,j};
        
        % 计算与所有词汇模板的DTW距离
        minDist_eval = inf;
        recognizedWordIndex_eval = -1;
        
        for k = 1:numWords
            if isempty(referenceTemplates{k})
                continue;
            end
            
            % 如果当前样本就是参考模板，跳过比较
            if k == i && j == 1 % 假设第一个样本是参考模板
                continue;
            end
            
            % 计算DTW距离
            [dist_eval, ~] = dtw(testFeatures_eval', referenceTemplates{k}');
            
            if dist_eval < minDist_eval
                minDist_eval = dist_eval;
                recognizedWordIndex_eval = k;
            end
        end
        
        if recognizedWordIndex_eval ~= -1
            totalCount = totalCount + 1;
            fprintf('  识别为: %s (实际: %s)\n', vocabulary{recognizedWordIndex_eval}, vocabulary{i});
            
            % 更新混淆矩阵
            confusionMatrix(i, recognizedWordIndex_eval) = confusionMatrix(i, recognizedWordIndex_eval) + 1;
            
            if recognizedWordIndex_eval == i
                correctCount = correctCount + 1;
            end
        end
    end
end

% 输出识别准确率
if totalCount > 0
    accuracy = (correctCount / totalCount) * 100;
    fprintf('整体识别准确率: %.2f%% (%d / %d)\n', accuracy, correctCount, totalCount);
else
    disp('没有样本用于评估。');
end

% 可视化混淆矩阵
figure;
imagesc(confusionMatrix);
colorbar;
title('混淆矩阵');
xlabel('识别结果');
ylabel('实际类别');
xticks(1:numWords);
yticks(1:numWords);
xticklabels(vocabulary);
yticklabels(vocabulary);
xtickangle(45);
for i = 1:numWords
    for j = 1:numWords
        if confusionMatrix(i,j) > 0
            text(j, i, num2str(confusionMatrix(i,j)), 'HorizontalAlignment', 'center');
        end
    end
end

disp('--- 程序结束 ---');

%% --- 端点检测函数 ---
function [startIndex, endIndex] = endpointDetection(signal, fs, frameLength, frameShift, energyThreshold, zcrThreshold)
% endpointDetection: 使用短时能量和短时过零率进行语音信号的端点检测
% 输入:
%   signal - 输入的语音信号 (列向量)
%   fs - 信号的采样率 (Hz)
%   frameLength - 帧长 (ms)
%   frameShift - 帧移 (ms)
%   energyThreshold - 能量阈值 (用于区分语音和静音)
%   zcrThreshold - 过零率阈值 (用于区分清音和浊音/静音)
% 输出:
%   startIndex - 语音段的起始采样点索引
%   endIndex - 语音段的结束采样点索引

% 将时间和ms转换为样本数
frameLengthSamples = round(fs * frameLength / 1000);
frameShiftSamples = round(fs * frameShift / 1000);

signal = signal / max(abs(signal)); % 幅值归一化

% --- 1. 计算短时能量 ---
numFrames = floor((length(signal) - frameLengthSamples) / frameShiftSamples) + 1;
energy = zeros(1, numFrames);
for i = 1:numFrames
    startSample = (i-1) * frameShiftSamples + 1;
    endSample = startSample + frameLengthSamples - 1;
    frame = signal(startSample:min(endSample, length(signal)));
    energy(i) = sum(frame.^2);
end
energy = energy / max(energy); % 能量归一化

% --- 2. 计算短时过零率 ---
zcr = zeros(1, numFrames);
for i = 1:numFrames
    startSample = (i-1) * frameShiftSamples + 1;
    endSample = startSample + frameLengthSamples - 1;
    frame = signal(startSample:min(endSample, length(signal)));
    zcr(i) = sum(abs(diff(sign(frame)))) / (2 * length(frame));
end
zcr = zcr / max(zcr); % 过零率归一化，如果需要的话

% --- 3. 端点检测逻辑 (一个简单的基于双阈值的方法) ---
% 这里使用一个简化的逻辑，实际应用中可能需要更复杂的算法

% 通常，语音的能量较高，过零率在清音段较高，浊音段较低
% 静音段能量和过零率都较低

% 找到可能的语音帧 (能量高于阈值)
speechFramesEnergy = energy > energyThreshold;
% 找到可能的语音帧 (过零率高于阈值，辅助判断)
% speechFramesZCR = zcr > zcrThreshold; % 这个阈值需要仔细调整

% 简单逻辑：连续N帧满足条件则认为是语音段

% 使用一个滑动窗口进行平滑，去除孤立的噪声点
windowSize = 5; % 平滑窗口大小，可以调整
speechSegments = conv(double(speechFramesEnergy), ones(1, windowSize)/windowSize, 'same') > 0.5; % 多数投票

% 查找第一个和最后一个语音段的边界
startFrame = -1;
endFrame = -1;

inSpeech = false;
for i = 1:numFrames
    if speechSegments(i) && ~inSpeech
        if startFrame == -1 % 记录第一个语音段的开始
            startFrame = i;
        end
        inSpeech = true;
    elseif ~speechSegments(i) && inSpeech
        endFrame = i-1; % 记录当前语音段的结束 (前一帧)
        % 这里可以加入逻辑：如果语音段太短，则忽略
        % if (endFrame - startFrame + 1) * frameShiftSamples < minSpeechLengthSamples
        %     startFrame = -1; % 重置
        %     endFrame = -1;
        % end
        inSpeech = false;
        % 简单起见，我们取第一个检测到的语音段的开始和最后一个检测到的语音段的结束
        % 如果需要处理多个语音段，逻辑会更复杂
    end
end

% 如果语音一直持续到最后
if inSpeech && startFrame ~= -1 && endFrame == -1
    endFrame = numFrames;
end

% 如果没有检测到语音
if startFrame == -1 || endFrame == -1 || startFrame >= endFrame
    warning('未能检测到有效语音段，将返回整个信号。');
    startIndex = 1;
    endIndex = length(signal);
    return;
end

% 将帧索引转换为采样点索引
startIndex = (startFrame - 1) * frameShiftSamples + 1;
endIndex = (endFrame - 1) * frameShiftSamples + frameLengthSamples; % 最后一帧的结束

% 确保索引在信号范围内
startIndex = max(1, startIndex);
endIndex = min(length(signal), endIndex);

% % 可选：可视化，用于调试
% figure;
% t_signal = (0:length(signal)-1)/fs;
% subplot(3,1,1);
% plot(t_signal, signal); title('原始语音信号');
% hold on;
% line([startIndex/fs, startIndex/fs], ylim, 'Color', 'r', 'LineWidth', 1.5);
% line([endIndex/fs, endIndex/fs], ylim, 'Color', 'r', 'LineWidth', 1.5);
% legend('Signal', 'Start', 'End');
% ylabel('幅度');
% 
% t_frames = (0:numFrames-1) * frameShift / 1000;
% subplot(3,1,2);
% plot(t_frames, energy); title('短时能量');
% hold on;
% plot(t_frames, speechFramesEnergy*max(energy), 'g--');
% plot(t_frames, speechSegments*max(energy)*0.8, 'm:');
% line([startFrame*frameShift/1000, startFrame*frameShift/1000], ylim, 'Color', 'r');
% line([endFrame*frameShift/1000, endFrame*frameShift/1000], ylim, 'Color', 'r');
% legend('Energy', 'Energy > Threshold', 'Smoothed Segment');
% ylabel('归一化能量');
% 
% subplot(3,1,3);
% plot(t_frames, zcr); title('短时过零率');
% % hold on;
% % line([startFrame*frameShift/1000, startFrame*frameShift/1000], ylim, 'Color', 'r');
% % line([endFrame*frameShift/1000, endFrame*frameShift/1000], ylim, 'Color', 'r');
% xlabel('时间 (s)'); ylabel('归一化过零率');
end 