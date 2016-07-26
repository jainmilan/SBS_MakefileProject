set I;
param th1;
param th2;
param th3;
param th4;
param th5;
param C1;
param C2;
param tau;
param alp1;
param alp0;
param rs;
param ql;
param qocp;
param qspot;
param a0;
param a1;
param b1;
param d1;
param d2;
param d3;
param a2;
param b2;
param d4;
param ulim1;
param ulim2;
param vlim1;
param volim1;
param vlim2;
param n1; # type 1 rooms
param n2; # type 2 rooms
param betalim1_single;
param betalim2_single;
param betalim1{1..n1};
param betalim2{1..n1};
param betalim11{1..n1};
param betalim12{1..n1};
param N;
param T;
param To{1..144+(6*(N-1))};
param O{1..144+(6*(N-1)),I};
param P0{I};
param delP0{I};
param l1;
param l2;
param plim1;
param plim2;
param U;
param bf1;
param power_hvac;
param power_spot;
param power_total;
param bf3;
#Variables#
var u{1..T} >=0, default 35 ;
var w{1..n1,1..T} >=0, default 0;
var v{1..T} >=0, default 0.236 ;
var P{I,1..T+1} >=0, default 22;
var delP{1..n1,1..T+1} >=0, default 0;
var Pm{1..n1,2..T+1}, default 0;
var r{1..T}>=0 ,default 0.8;
var Tia{1..T}>=0, default 25 ;
var Tc{1..T}>=0, default 25;
var va{1..n1,1..T}>=0;

# Objective #

minimize energy:sum{t in 1..T}((v[t]*th1*(u[t]-Tc[t])) +(v[t]*th2*(Tia[t]-Tc[t]))+ (th3*v[t]^2))
 + sum{t in 1..T, k in 1..n1}(th4*w[k,t]+th5*va[k,1]);

subject to initial0 {k in 1..5}:P[k,1]=P0[k];

subject to dynamics0 {t in 1..T,k in 1..n1}: P[k,t+1]=a0*P[k,t] + a1*P[k,t]*v[t]/5 + b1*u[t]*v[t]/5 + d2*ql*O[6*(l1-1)+l2+t,k] 
     + d1*To[6*(l1-1)+l2+t] + d3*delP[k,t]; 
     
subject to dynamics_comroom {t in 1..T,k in n1+1..n1+n2}: P[k,t+1]=a0*P[k,t] + a1*P[k,t]*v[t]/5 + b1*u[t]*v[t]/5 
  + d2*(ql+qocp)*O[6*(l1-1)+l2+t,k]  + d1*To[6*(l1-1)+l2+t];

subject to initial1 {k in 1..n1}:delP[k,1]=delP0[k];
subject to dynamics1 {t in 1..T,k in 1..n1}:delP[k,t+1]=a2*delP[k,t] + w[k,t]*b2*qspot + d4*qocp*O[6*(l1-1)+l2+t,k];



subject to cons1 {t in 1..T}: Tia[t]  =r[t]*(sum{k in I} P[k,t])/5 + (1-r[t])*To[6*(l1-1)+l2+t]; 
subject to cons2 {t in 2..T+1, k in 1..n1}: Pm[k,t]= O[6*(l1-1)+l2+t-1,k]*(0.2466*(P[k,t]+delP[k,t]-d3*delP[k,t-1]) + 0.5810*va[k,t-1]*va[k,t-1] 
       -1.4075*va[k,t-1] - 5.4668);

#subject to pmvlt1 {t in 2..T+1, k in 1..n1} :  O[6*(l1-1)+l2+t-1,k]*Pm[k,t] >= O[6*(l1-1)+l2+t-1,k]*betalim1[k];
#subject to pmvlt2 {t in 2..T+1, k in 1..n1} : O[6*(l1-1)+l2+t-1,k]*Pm[k,t] <= O[6*(l1-1)+l2+t-1,k]*betalim2[k];

subject to pmvlt1 {t in 2..T+1, k in 1..n1} :  Pm[k,t] >= betalim1_single;
subject to pmvlt2 {t in 2..T+1, k in 1..n1} : Pm[k,t] <= betalim2_single;

#subject to pmvlt1a {t in 2..T+1, k in 1..n1} : O[6*(l1-1)+l2+t-1,k]*P[k,t] >=O[6*(l1-1)+l2+t-1,k]*betalim11;
#subject to pmvlt2a {t in 2..T+1, k in 1..n1} : O[6*(l1-1)+l2+t-1,k]*P[k,t] <=O[6*(l1-1)+l2+t-1,k]*betalim12;


#subject to pmvlt3 {t in 2..T+1,k in  n1+1..n1+n2} :O[6*(l1-1)+l2+t-1,k]*P[k,t] >=O[6*(l1-1)+l2+t-1,k]*betalim11;
#subject to pmvlt4 {t in 2..T+1,k in n1+1..n1+n2} :O[6*(l1-1)+l2+t-1,k]*P[k,t] <=O[6*(l1-1)+l2+t-1,k]*betalim12;

subject to ipcons1 {i in 0..(N-1),j in 1..5}:bf1*u[6*i+1]=bf1*u[6*i+j+1];
subject to ipcons2 {j in 0..(5-l2)}:l2*u[1]=l2*u[1+j];
subject to ipcons3 {i in 0..(N-2), j in 1..5}:l2*u[6*(i+1)-l2+1]=l2*u[6*(i+1)-l2+j+1];
subject to ipcons4 {i in (N-1)..(N-1),j in 0..(bf3-1)}:l2*u[6*(i+1)-bf3+1]=l2*u[6*(i+1)-bf3+1+j];
subject to ipcons5 :l2*u[1]=l2*U;
#subject to ipcons6 {t in 1..T,k in 1..n1}:delP[k,t]<=4;


subject to voclt {t in 1..T} : (sum{k in I}O[6*(l1-1)+l2+t,k])*v[t] >= (sum{k in I}O[6*(l1-1)+l2+t,k])*volim1;

subject to plt1 {t in 2..T+1,k in I} : P[k,t]>=plim1;
subject to plt2 {t in 2..T+1,k in I} : P[k,t]<=plim2;

subject to vlt1 {t in 1..T} : v[t]>=vlim1;
subject to vlt2 {t in 1..T}: v[t]<=vlim2;
subject to ult1 {t in 1..T} : u[t]>=ulim1;
subject to ult2 {t in 1..T} : u[t]<=ulim2;
subject to rlt {t in 1..T}: r[t]<=0.8;
subject to urlt1 {t in 1..T} : u[t]>=Tc[t];
subject to urlt2 {t in 1..T} : Tc[t]<=Tia[t];
subject to wlt {t in 1..T,k in 1..n1} : w[k,t]<=O[6*(l1-1)+l2+t,k];
subject to valt1 {t in 1..T,k in 1..n1} : va[k,t]=0;#<=O[6*(l1-1)+l2+t,k]*1;
#subject to single {t in 1..T,k in 1..n1}: va[k,t]*w[k,t]=0;







