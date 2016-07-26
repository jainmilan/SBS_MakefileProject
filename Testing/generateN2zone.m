function[]=generateN2zone(O,To,P0,N1,flag);
n=1;
C1=2000;
C2=200;
tau=600;
alp1=0.1425;
alp0=0.048;
rs=1.2041;
ql=0.1;
qocp=0.1;
qspot=0.7;
 a0=1-(tau*alp0/C1);
 a1=-tau*rs/C1;
 a2=1-(tau*alp1/C2);
 b1=tau*rs/C1;
 d1=tau*alp0/C1;
 d2=tau/C1;
 d3=tau*alp1/C1;
 d4=tau/C2;
 b2=tau/C2;

U=12:35;
V=[0.236 0.4  0.6  0.8  1  1.2  1.4  1.6 1.8 2 2.2 2.4 2.6 2.8 3 3.2 3.4 3.6 3.8 4];
R=0:8;
W=0:0.1:1;
Pval=18:28;
Pmval=-0.5:0.1:0.5;

a=randi(size(U,2),N1*n,1);
d=randi(size(V,2),6*N1*n,1);
c=randi(size(R,2),6*N1*n,1);
f=randi(size(Pval,2),6*N1*n+1,1);
g=randi(size(Pmval,2),6*N1*n,1);

for i =1:n
    for j=0:N1-1
        for k =1:6
         if flag==0
            Uval(i,6*j+k)=0;
            else
            Uval(i,6*j+k)=U(1,a(N1*(i-1)+j+1));
         end
        end
    end

end


for i =1:n
    for j=0:6*N1-1
    if flag==0
            Vval(i,6*j+k)=0;
            else
            Vval(i,j+1)=V(1,d(6*N1*(i-1)+j+1));
            end
    end
end


for i =1:n
    for j=1:6*N1
    if flag==0
            Rval(i,6*j+k)=0;
            else
       Rval(i,j)=R(1,c(6*N1*(i-1)+j))/10;
       end
    end
end
p=1;
fid = fopen('var_value.dat', 'w');
        fprintf(fid, 'var u:=');
        for i=1:(6*N1)
           fprintf(fid, '\n %d  %f', i,Uval(p,i));
        end
        fprintf(fid, ';');
        fprintf(fid, '\n var v:=');
        for i=1:(6*N1)
           fprintf(fid, '\n%d  %f', i,Vval(p,i));
        end
        fprintf(fid, ';');
        
        fprintf(fid, '\n var r:=');
        for i=1:6*N1
           fprintf(fid, '\n %d  %f', i,Rval(p,i));
        end
        fprintf(fid, ';');
        
        fprintf(fid, '\n var w:');        
        for i=1:6*N1
        fprintf(fid, '\t %d',i);
        end
        fprintf(fid, '\t :=');
        for k=1:5
        fprintf(fid,'\n %d',k); 
           for i=1:6*N1
                if flag==0
                Wval(i,6*j+k)=0;
                va(k,i)=0;
                else
                %Wval(i,6*j+k)=0;
                Wval(k,i)=W(1,randi(size(W,2)));
                %va(k,i)= W(1,randi(size(W,2)));
                va(k,i)=0;
                end
          fprintf(fid,'\t %f', Wval(k,i));
        %if Wval(k,i)>0 
        %va(k,i)=1;
       %else 
       %va(k,i)=0;
       %va(k,i)= W(1,randi(size(W,2)));
      %end
         end
        end
         fprintf(fid, ';'); 
        fprintf(fid, '\n var P:');
        for i=1:6*N1+1
        fprintf(fid, '\t %d',i);
        end
        fprintf(fid, '\t :=');
        
        for k=1:5
        if flag==0
        P(k,1)=0;
        else
         P(k,1)=P0(k,1);
         end
         end
        for k=1:5
             fprintf(fid,'\n %d',k);     
             fprintf(fid,'\t %f',P(k,1));                         
             for i =1:6*N1  
             if flag ==0
             P(k,i+1)=0;
             elseif flag==1
              P(k,i+1)=Pval(1,f(i+1));
             else
            P(k,i+1)=a0*P(k,i) + a1*P(k,i)*Vval(p,i)/5 + b1*Uval(p,i)*Vval(p,i)/5 + d2*(ql+qocp)*O(i,k) + d1*To(i,1) + d2*qspot*Wval(k,i);
             end
      fprintf(fid,'\t %f', P(k,i+1));
             end
        end
        fprintf(fid, ';');
        
                
       fprintf(fid, '\n var Tia:=');
        for i=1:(6*N1)
           fprintf(fid, '\n %d  %f', i,Uval(p,i));
        end
        fprintf(fid, ';'); 
        fprintf(fid, '\n var Tc:=');
        for i=1:(6*N1)
           fprintf(fid, '\n %d  %f', i,Uval(p,i));
        end
        fprintf(fid, ';'); 
        
        fprintf(fid, '\n var Pm:');
        for i=2:(6*N1+1)
        fprintf(fid, '\t %d',i);
        end
        fprintf(fid, '\t :=');
        for k=1:5
             fprintf(fid,'\n %d',k);                   
             for i=2:(6*N1+1) 
             if flag==0
             Pm(k,i)=0;
             elseif flag==1
             Pm(k,i)=Pmval(1,g(i-1));
             else
           Pm(k,i)=0.2466*P(k,i) + 0.5810*va(k,i-1)*va(k,i-1) -1.4075*va(k,i-1) - 5.4668;
           %Pm(k,i)=0.3684*P(k,i) + 0.7548*va(k,i-1)*va(k,i-1) -2.1438*va(k,i-1) - 9.2129;
             end
       fprintf(fid,'\t %f',Pm(k,i));
          end
        end
        fprintf(fid, ';');   
        
        fprintf(fid, '\n var va:');
        for i=1:(6*N1)
        fprintf(fid, '\t %d',i);
        end
        fprintf(fid, '\t :=');
        for k=1:5
             fprintf(fid,'\n %d',k);
        for i=1:6*N1
      fprintf(fid,'\t %f',va(k,i));
      end
      end
      fprintf(fid, ';');
       fclose(fid);
        

