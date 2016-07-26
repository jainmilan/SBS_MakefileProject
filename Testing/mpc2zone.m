function[]=mpc2zone(x,y,z);
if z==1
m1=1;
fid = fopen('out1.dat', 'w');
       fprintf(fid, '\n');
       fclose(fid);
elseif z==2
m1=15;
fid = fopen('out.dat', 'w');
       fprintf(fid, '\n');
       fclose(fid);
end
N=4;
To1=load('Temp_winter1.dat'); 
O1=load('ocpdata.dat');

%%%To change 4 as common room%%%%%
O2=O1;
O1=O2(:,[1 2 3 5 4]);

d1=x;
d2=y;
To=To1([144*(d1-1)+1 : 144*(d1-1)+144+(6*(N-1))],1);
O=O1([144*(d2-1)+1 : 144*(d2-1)+144+(6*(N-1))],:);

fid = fopen('temperature.dat', 'w');
      fprintf(fid, 'param To := ');
      fclose(fid);
te=1:144+(6*(N-1));
temp=[te' To];
dlmwrite('temperature.dat', temp,'delimiter','\t','-append');
fid = fopen('temperature.dat', 'a');
fprintf(fid, ';');
fclose(fid);
fid = fopen('occupancy.dat', 'w');
fprintf(fid, 'param O : 1 2 3 4 5 :=');
fclose(fid);
occp=[te' O];
dlmwrite('occupancy.dat', occp,'delimiter','\t','-append');
fid = fopen('occupancy.dat', 'a');
fprintf(fid, ';');
fclose(fid);

fid = fopen('Pinvalue.dat', 'w');
       fprintf(fid, '\n param P0:=');
       for k=1:5
        fprintf(fid, '\n %d %f',k, 21);
        P0(k,1)=21;
        end 
       fprintf(fid, ';');  
       fclose(fid);
       
       fid = fopen('delPvalue.dat', 'w');
       fprintf(fid, '\n param delP0:=');
       for k=1:5
        fprintf(fid, '\n %d %f',k, 0);
        delP0(k,1)=0;
        end 
       fprintf(fid, ';');  
       fclose(fid);
for i=1:23
 fid = fopen('inp_value.dat', 'w');
       fprintf(fid, '\n param l1 := %d',i);
       fprintf(fid, ';');
       fprintf(fid, '\n param l2 := %d',0);
       fprintf(fid, ';');
       fclose(fid);
       
       fid = fopen('out_Nguess.dat', 'w');
       fprintf(fid, '\n');
       fclose(fid);
       for m=1:m1
       x1=O([(6*(i-1)+1):(6*(i-1)+6*N)],:);
       x2=To([(6*(i-1)+1):(6*(i-1)+6*N)],:);
       if z==1
       generateN2zone(x1,x2,P0,N,2);
       %generateN_rand(N);
       dos('ampl mpc2zone.ampl');
       elseif z==2
       generateN1zone(x1,x2,P0,N);
       dos('ampl mpc1zone.ampl');
       end
       end
       Min=load('out_Nguess.dat');
       %Min=Min(Min(:,1)==0,:);
       %if length(Min)==0
       %Min=load('out_Nguess.dat');
       %Min=Min(Min(:,4)>=0,:);
       %end
       power=min(Min(:,4));
       %powermax=max(Min(:,4));
       min1=Min(Min(:,4)==power,:);
       min1=min1(min1(:,24)==min(min1(:,24)),:);
       
       %if power==0
       %min1=Min((Min(:,4)-power)<0.05,:);
       %else
       %min1=Min((Min(:,4)-power)/power<0.05,:);
       %end
       
 %fid = fopen('out_Nguess.dat', 'w');
 %      fprintf(fid, '\n');
 %      fclose(fid);      
 %generateN2zone(x1,x2,P0,N,0);
%dos('ampl mpc2zone.ampl');
%Min_0=load('out_Nguess.dat');
%Min_1=zeros(1,6);
%if Min_0(1,1)>0
%fid = fopen('out_Nguess.dat', 'w');
%       fprintf(fid, '\n');
%       fclose(fid);
% generateN2zone(x1,x2,P0,N,1);
%dos('ampl mpc2zone.ampl');
%Min_1=load('out_Nguess.dat');
%end
%if Min_1(1,1)>0
%fid = fopen('out_Nguess.dat', 'w');
%       fprintf(fid, '\n');
%       fclose(fid);
% generateN2zone(x1,x2,P0,N,1);
%dos('ampl mpc2zone.ampl'); 
%Min_1=load('out_Nguess.dat');
%end 
%if Min_1(1,1)>0
%fid = fopen('out_Nguess.dat', 'w');
%       fprintf(fid, '\n');
%       fclose(fid);
% generateN2zone(x1,x2,P0,N,1);
%dos('ampl mpc2zone.ampl'); 
%Min_1=load('out_Nguess.dat'); 
%end
%if Min_1(1,1)>0
%fid = fopen('out_Nguess.dat', 'w');
%       fprintf(fid, '\n');
%       fclose(fid);
% generateN2zone(x1,x2,P0,N,1);
%dos('ampl mpc2zone.ampl'); 
%Min_1=load('out_Nguess.dat');
%end
  
%fid = fopen('out_1guessvs1000guess_winter_1.dat', 'a');
%fprintf(fid, '\n %d %d %f %f %f %f %f %f %f %f',i,0,power,powermax,size(Min,1),size(min1,1),Min_0(1,1),Min_0(1,4),Min_1(1,1),Min_1(1,4));      
%       fclose(fid);  
        
%  min1=min1(min1(:,24)==min(min1(:,24)),:);
     
   fid = fopen('Pinvalue.dat', 'w');
       fprintf(fid, '\n param P0:=');
       for k=1:5
        fprintf(fid, '\n %d %f',k, min1(1,9+k));
        P0(k,1)=min1(1,9+k);
        end 
       fprintf(fid, ';');  
       fclose(fid);       
       
       fid = fopen('Uvalue.dat', 'w');
       fprintf(fid, '\n param U:=');
       fprintf(fid, '\n %f', min1(1,15));
       fprintf(fid, ';');  
       fclose(fid);           
       if z==1   
       fid = fopen('delPvalue.dat', 'w');
       fprintf(fid, '\n param delP0:=');
       for k=1:5
        fprintf(fid, '\n %d %f',k, min1(1,24+k));
        delP0(k,1)=min1(1,24+k);
        end 
       fprintf(fid, ';');  
       fclose(fid);  
         
       fid = fopen('out1.dat', 'a');
       fprintf(fid, '\n %d',min1(1,1));
       fprintf(fid, '\t %d',min1(1,2));
       fprintf(fid, '\t %d',min1(1,3));
       fprintf(fid, '\t %f',min1(1,15));
       fprintf(fid, '\t %f',min1(1,16));
       fprintf(fid, '\t %f',min1(1,17));
       fprintf(fid, '\t %f',min1(1,24));
       fprintf(fid, '\t %f',min1(1,30));
       fprintf(fid, '\t %f',min1(1,31));
       for k=1:5
        %fprintf(fid, '\t %d %f',k,min1(1,9+k)+min1(1,24+k));
        fprintf(fid, '\t  %f',min1(1,18+k));
       end 
       for k=1:5
        fprintf(fid, '\t  %f',min1(1,4+k));
       end 
       for k=1:5
        fprintf(fid, '\t  %f',min1(1,31+k));
       end
       for g=1:5
       %fprintf(fid, '\t %d',O(6*(i-1)+1,g));
       fprintf(fid, '\t %f',min1(1,9+g));
       %fprintf(fid, '\t %f',min1(1,9+g)+min1(1,24+g));
       end
       fclose(fid);
       elseif z==2
       fid = fopen('out.dat', 'a');
       fprintf(fid, '\n %d',min1(1,1));
       fprintf(fid, '\t %d',min1(1,2));
       fprintf(fid, '\t %d',min1(1,3));
       fprintf(fid, '\t %f',min1(1,24));
       fprintf(fid, '\t %f',min1(1,15));
       for k=1:5
        fprintf(fid, '\t %f',min1(1,9+k));
       end 
       %for g=1:5
       %fprintf(fid, '\t %d',O(6*(i-1)+1,g));
       %fprintf(fid, '\t %f',min1(1,4+g)+min1(1,24+g));
       %end
       %fclose(fid);
    end

 for j=1:5
  fid = fopen('inp_value.dat', 'w');
       fprintf(fid, '\n param l1 := %d',i);
       fprintf(fid, ';');
       fprintf(fid, '\n param l2 := %d',j);
       fprintf(fid, ';');
       fclose(fid);
       
       fid = fopen('out_Nguess.dat', 'w');
       fprintf(fid, '\n');
       fclose(fid);
       for m=1:m1
       if z==1
       %generateN_rand(N);
       generateN2zone(O([6*(i-1)+1+j:6*(i-1)+6*N+j],:),To([6*(i-1)+1+j:6*(i-1)+6*N+j],:),P0,N,2);
       dos('ampl mpc2zone.ampl');
       elseif z==2
       generateN1zone(O([6*(i-1)+1+j:6*(i-1)+6*N+j],:),To([6*(i-1)+1+j:6*(i-1)+6*N+j],:),P0,N);
       dos('ampl mpc1zone.ampl');
       end
       end
       Min=load('out_Nguess.dat');
       %Min=Min(Min(:,1)==0,:);
       %if length(Min)==0
       %Min=load('out_Nguess.dat');
       %Min=Min(Min(:,4)>=0,:);
       %end
       power=min(Min(:,4));
       powermax=max(Min(:,4));
       min1=Min(Min(:,4)==power,:);
       min1=min1(min1(:,24)==min(min1(:,24)),:);
       
       %if power==0
       %min1=Min((Min(:,4)-power)<0.05,:);
       %else
       %min1=Min((Min(:,4)-power)/power<0.05,:);
       %end
       
%fid = fopen('out_Nguess.dat', 'w');
%       fprintf(fid, '\n');
%       fclose(fid);       
%generateN2zone(x1,x2,P0,N,0);
%dos('ampl mpc2zone.ampl');
%Min_0=load('out_Nguess.dat');
%Min_1=zeros(1,6);
%if Min_0(1,1)>0
%fid = fopen('out_Nguess.dat', 'w');
%       fprintf(fid, '\n');
%       fclose(fid);
% generateN2zone(x1,x2,P0,N,1);
%dos('ampl mpc2zone.ampl');
%Min_1=load('out_Nguess.dat');
%end
%if Min_1(1,1)>0
%fid = fopen('out_Nguess.dat', 'w');
%       fprintf(fid, '\n');
%       fclose(fid);
% generateN2zone(x1,x2,P0,N,1);
%dos('ampl mpc2zone.ampl'); 
%Min_1=load('out_Nguess.dat');
%end 
%if Min_1(1,1)>0
%fid = fopen('out_Nguess.dat', 'w');
%       fprintf(fid, '\n');
%       fclose(fid);
% generateN2zone(x1,x2,P0,N,1);
%dos('ampl mpc2zone.ampl'); 
%Min_1=load('out_Nguess.dat'); 
%end
%if Min_1(1,1)>0
%fid = fopen('out_Nguess.dat', 'w');
%       fprintf(fid, '\n');
%       fclose(fid);
% generateN2zone(x1,x2,P0,N,1);
%dos('ampl mpc2zone.ampl'); 
%Min_1=load('out_Nguess.dat');
%end
  
%fid = fopen('out_1guessvs1000guess_winter_1.dat', 'a');
%fprintf(fid, '\n %d %d %f %f %f %f %f %f %f %f',i,j,power,powermax,size(Min,1),size(min1,1),Min_0(1,1),Min_0(1,4),Min_1(1,1),Min_1(1,4));      
%       fclose(fid);  
        
  min1=min1(min1(:,24)==min(min1(:,24)),:);
       
       fid = fopen('Pinvalue.dat', 'w');
       fprintf(fid, '\n param P0:=');
       for k=1:5
        fprintf(fid, '\n%d %f',k, min1(1,9+k));
         P0(k,1)=min1(1,9+k);
        end 
       fprintf(fid, ';');  
       fclose(fid);
       
       if z==1 
       fid = fopen('delPvalue.dat', 'w');
       fprintf(fid, '\n param delP0:=');
       for k=1:5
        fprintf(fid, '\n %d %f',k, min1(1,24+k));
        delP0(k,1)=min1(1,24+k);
        end 
       fprintf(fid, ';');  
       fclose(fid);      
       fid = fopen('out1.dat', 'a');
       fprintf(fid, '\n %d',min1(1,1));
       fprintf(fid, '\t %d',min1(1,2));
       fprintf(fid, '\t %d',min1(1,3));
       fprintf(fid, '\t %f',min1(1,15));
       fprintf(fid, '\t %f',min1(1,16));
       fprintf(fid, '\t %f',min1(1,17));
       fprintf(fid, '\t %f',min1(1,24));
        fprintf(fid, '\t %f',min1(1,30));
       fprintf(fid, '\t %f',min1(1,31));
       for k=1:5
        %fprintf(fid, '\t %d %f',k,min1(1,9+k)+min1(1,24+k));
        fprintf(fid, '\t %f',min1(1,18+k));
       end 
       for k=1:5
        fprintf(fid, '\t  %f',min1(1,4+k));
       end 
       for k=1:5
        fprintf(fid, '\t  %f',min1(1,31+k));
       end
       for g=1:5
       %fprintf(fid, '\t %d',O(6*(i-1)+1+j,g));
       fprintf(fid, '\t %f',min1(1,9+g));
       %fprintf(fid, '\t %f',min1(1,9+g)+min1(1,24+g));
       end
       fclose(fid);
       elseif z==2
       fid = fopen('out.dat', 'a');
       fprintf(fid, '\n %d',min1(1,1));
       fprintf(fid, '\t %d',min1(1,2));
       fprintf(fid, '\t %d',min1(1,3));
       fprintf(fid, '\t %f',min1(1,24));
       fprintf(fid, '\t %f',min1(1,15));
       for k=1:5
        fprintf(fid, '\t %f',min1(1,9+k));
       end 
       %for g=1:5
       %fprintf(fid, '\t %d',O(6*(i-1)+1+j,g));
       %fprintf(fid, '\t %f',min1(1,4+g));
       %end
       fclose(fid);
    end
 end
end
