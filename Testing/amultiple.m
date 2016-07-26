fid = fopen('out_Nguess1.dat', 'w');
       fprintf(fid, '\n');
       fclose(fid);
N=4;% 12
for i=1:1;
for j=1:1;
%generateN_rand(N);
mpc2zone(i,j,1); %spot
%mpc2zone(i,j,2); % no spot
%mpc1(i,j,1); %spot
%mpc1(i,j,2); % no spot
%multizone(i,j,1);
%multizone(i,j,2);
%multizone_summer(i,j,1);
%multizone_summer(i,j,2);
a=load('out.dat');
b=load('out1.dat'); %spot
a1=sum(a,1);
b1=sum(b,1);
%fid = fopen('Result_multizone.dat', 'a+');
%       fprintf(fid, '\n %d %d %f %f %f %f',i,j,a1(1,4),b1(1,4),a1(1,1),b1(1,1));
%       fclose(fid);
       
       
 %fid = fopen('Result_summer.dat', 'a+');
 %     fprintf(fid, '\n %d %d %f %f %f %f',i,j,a1(1,4),b1(1,4),a1(1,1),b1(1,1));
 %      fclose(fid);
       
 % fid = fopen('Result_summer.dat', 'a+');
 %     fprintf(fid, '\n %d %d %f %f ',i,j,b1(1,4),b1(1,1));
 %      fclose(fid);
end;
end;
