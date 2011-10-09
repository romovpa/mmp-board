%CHECKSTEP_HORSES(PREV,CUR,TYPE)
%
% step checker for the game "Horses"
%
% See CHECKSTEP()
%
function [outcome, description, move] = checkstep_horses(prev, cur, type)

player = prev.player;
opponent = cur.player;

% existence of pieces
A_pieces = find(cur.matrix=='A' | cur.matrix=='a');
B_pieces = find(cur.matrix=='B' | cur.matrix=='b');
if isempty(A_pieces) || isempty(B_pieces)
  outcome = -1;
  description = 'One player missing';
  return;
end

% checking movement
changes = find(prev.matrix ~= cur.matrix);
if length(changes) ~= 2
  outcome = -1;
  description = 'Incorrect movement';
  return;
end

% not a piece movement
if cur.matrix(changes(1)) ~= '-'
  changes = [changes(2) changes(1)];
end
if (cur.matrix(changes(1))~='-') || ...
    ~strcmpi(cur.matrix(changes(2)), char('A'+player-1))
  outcome = -1;
  description = 'Not a piece movement';
  return;
end

[row, col] = ind2sub(size(cur.matrix), changes);
move = [reshape(row,2,1) reshape(col,2,1)];

% piece moved not as a horse
delta = sort([ abs(row(1)-row(2)) abs(col(1)-col(2)) ]);
if ~issame(delta, [1 2])
  outcome = -1;
  description = 'Piece moved not as a horse';
  return;
end

% player changed opponent's score
if cur.score(opponent) ~= prev.score(opponent)
  outcome = -1;
  description = 'Player changed opponent''s score';
  return;
end

ex_score = prev.score;

% eating
eated = prev.matrix(changes(2));
if eated == char('A'+opponent-1)
  ex_score(player) = ex_score(player) + 1;
end
if ismember(eated, '12')
  ex_score(player) = ex_score(player) + eated-'0';
end

% parade of pieces
over = 0;
[A_rows, A_cols] = ind2sub(size(cur.matrix), A_pieces);
[B_rows, B_cols] = ind2sub(size(cur.matrix), B_pieces);
A_rows = unique(A_rows);
A_cols = unique(A_cols);
B_rows = unique(B_rows);
B_cols = unique(B_cols);
if (length(A_rows)==1 || length(A_cols)==1)
  over = 1;
  ex_score(1) = ex_score(1) + 3;
end
if (length(B_rows)==1 || length(B_cols)==1)
  over = 1;
  ex_score(2) = ex_score(2) + 3;
end

% checking player's score
if ~issame(ex_score, cur.score)
  outcome = -1;
  description = 'Scores updated incorrectly';
  return;
end

% checking for game over
if over 
  [~,ex_winner] = max(ex_score);
  if ex_winner ~= cur.winner
    outcome = -1;
    description = 'Game is over, winner changed incorrectly';
    return;
  end
end

outcome = cur.winner;
description = 'OK';