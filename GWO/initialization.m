%___________________________________________________________________%
%  Grey Wolf Optimizer (GWO) source codes version 1.0               %
%                                                                   %
%  Developed in MATLAB R2011b(7.13)                                 %
%                                                                   %
%  Author and programmer: Seyedali Mirjalili                        %
%                                                                   %
%         e-Mail: ali.mirjalili@gmail.com                           %
%                 seyedali.mirjalili@griffithuni.edu.au             %
%                                                                   %
%       Homepage: http://www.alimirjalili.com                       %
%                                                                   %
%   Main paper: S. Mirjalili, S. M. Mirjalili, A. Lewis             %
%               Grey Wolf Optimizer, Advances in Engineering        %
%               Software , in press,                                %
%               DOI: 10.1016/j.advengsoft.2013.12.007               %
%                                                                   %
%___________________________________________________________________%

% This function initialize the first population of search agents
function Positions=initialization(SearchAgents_no,dim,ub,lb,roll,pitch, yaw)

Boundary_no= size(ub,2); % numnber of boundaries

% If the boundaries of all variables are equal and user enter a signle
% number for both ub and lb
if Boundary_no==1
    Positions=rand(SearchAgents_no,dim).*(ub-lb)+lb;
end
ini = [1427.099976, 1449.400024, 987.900024, 579.400024, 1427.099976, 1449.400024, 987.900024, 579.400024,roll(2,1),yaw(2,1),pitch(2,1), roll(7,1),  yaw(7,1),pitch(7,1)]';
% If each variable has a different lb and ub
 Positions(1,:)=ini ;
if Boundary_no>1
    for j=2:SearchAgents_no
    for i=1:dim
        ub_i = ub(i);
        lb_i = lb(i);
        Positions(j,i) = rand(1,1).*(ub_i-lb_i)+lb_i;
    end
    end
end