%CHECKSTEP(PREV,CUR,TYPE)
%
% check halfstep correctness, returns game outcome
%
% PREV: game state before halfstep, or [] before the game started
% CUR:  game state after halfstep
% TYPE: name of the game, supported:
%           - horses
%           - mmpawns (not implemented)
%           - bfights (not implemented)
%
% Return:
%   -1   step is incorrect
%    0   continue
%    1   first wins
%    2   second wins
%    3   draw
%
function [outcome, descr, move] = checkstep(prev, cur, type)

MAX_FULLSTEPS = 30;
TIME_EPS = 1e-6;

% FIXME: case when prev==[]

player = prev.player;
opponent = cur.player;

% player didn't changed
if (player == opponent)
  outcome = -1;
  descr = 'Player did not changed';
  return;
end

% invalid halfsteps numbers
if (prev.halfstep <= 0) || (prev.halfstep+1 ~= cur.halfstep)
  outcome = -1;
  descr = 'Invalid halfstep numbers';
  return;
end

% second player didn't stop the game
% after the maximal full step number is reached
if (cur.halfstep > 2*MAX_FULLSTEPS) && (cur.winner == 0)
  outcome = -1;
  descr = 'Player B did not stopped game after max fullstep number is reached';
  return;
end

% game is already over
if (prev.winner ~= 0)
  outcome = -1;
  descr = 'Game is already over';
  return;
end

% player changed oppoennt's time
if (abs(cur.time(opponent)-prev.time(opponent)) > TIME_EPS)
  outcome = -1;
  descr = 'Player changed opponent''s time!';
  return;
end

% player increased its time
if (prev.time < cur.time)
  outcome = -1;
  descr = 'Player increased its time!';
  return;
end


% further checking
funcname = [ 'checkstep_' type ];
if ~exist(funcname, 'file')
  error('checkstep() is not supported for a game named "%s"', type);
end
checker = str2func(funcname);
[outcome, descr, move] = checker(prev, cur, type);
