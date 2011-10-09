%PLAYGAME(TYPE,COMMANDS,MATRIX)
%
% run the game
%
% TYPE:      name of the game
% COMMANDS:  cell array of commands to run player
% MATRIX:    matrix filename
%
function outcome = playgame(type, commands, matrix_filename, vis)

if ~exist('vis', 'var')
  vis = 0;
end

MATRIX_FILE = 'matrix.txt';

copyfile(matrix_filename, MATRIX_FILE);

prev = [];
cur = readgame(MATRIX_FILE);
outcome = 0; % checkstep(prev, cur, type);
while outcome == 0
  player = cur.player;
  fullstep = floor((cur.halfstep+1)/2);
  id = sprintf('%c_%d', 'A'-1+player, fullstep);
  fprintf('%s:\n', id);
  
  % run executable
  system(commands{player});
  
  % save result
  copyfile(MATRIX_FILE, ['matrix_' id '.txt']);
  
  % update outcome
  prev = cur;
  cur = readgame(MATRIX_FILE);
  [outcome,descr,move] = checkstep(prev, cur, type);
  
  % visualization
  if vis
    plotboard(cur.matrix, move);
    waitforbuttonpress;
  end
end

if outcome < 0
  fprintf('Incorrect step: %s\n', descr);
end

fprintf('Score: %d--%d\n', cur.score(1), cur.score(2));
switch outcome
  case 1
    fprintf('Player A wins!\n');
  case 2
    fprintf('Player B wins!\n');
  case 3
    fprintf('Draw!\n');
end