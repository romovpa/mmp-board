function run_battle(tests)
% left-vs-right fight testing
% tests: cell array of test file names
%        e.g.: {'one.txt', 'two.txt'}

game = 'horses';

tests_dir = '../tests/';
if ~exist('tests', 'var')
  tests_list = dir([tests_dir '*.txt']);
  tests = { tests_list.name };
end

right = {'../peter-ai'};
left = { '../peter-ai' };

wins = [0 0]; % [left right]

for l_idx = 1:length(left)
for r_idx = 1:length(right)
  l_cmd = left{l_idx};
  r_cmd = right{r_idx};
  
  for t_idx = 1:length(tests)
    mat_filename = fullfile(tests_dir, tests{t_idx});
    [~,mat_name] = fileparts(mat_filename);
    
    % A: left
    % B: right
    fprintf('== map: %s\n', mat_name);
    fprintf('A: %s\nB: %s\n', l_cmd, r_cmd);
    fprintf('----\n');
    outcome = playgame(game, {l_cmd, r_cmd}, mat_filename);
    if ismember(outcome, [1 2])
      wins(outcome) = wins(outcome) + 1;
    end
    fprintf('\n');
    
    % A: right
    % B: left
    fprintf('== map: %s\n', mat_name);
    fprintf('A: %s\nB: %s\n', r_cmd, l_cmd);
    fprintf('----\n');
    outcome = playgame(game, {r_cmd, l_cmd}, mat_filename);
    if ismember(outcome, [1 2])
      outcome = 3 - outcome;
      wins(outcome) = wins(outcome) + 1;
    end
    fprintf('\n');
  end
  
end
end

fprintf('Left wins: %d times\n', wins(1));
fprintf('Right wins: %d times\n', wins(2));
