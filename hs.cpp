#include <bits/stdc++.h>
#include <utility>
#include <vector>
#include <sys/types.h> 	
#include <arpa/inet.h>								//required libraries
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <random>
#include <chrono>
#include <thread>
#include <sys/wait.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <netdb.h>
using namespace std;

//type helpers
#define ll long long
#define vi vector <int>
#define vl vector <ll> 
#define pii pair<int, int>
#define pll pair<ll, ll>
#define vpii vector <pii >
#define vpll vector <pll >
//debugging
#define debug(x) cout<<"debugging "<<x<<endl

//loops
#define FOR(i,a,b) for(ll i=a;i<b;i++)
#define FORD(i,a,b) for(ll i=a-1;i>=b;i--)
#define iter(it, s) for(auto it=s.begin(); it!=s.end(); it++)

//io helpers
#define sd(n) scanf("%d", &n)
#define sl(n) scanf("%lld", &n)
#define slf(n) scanf("%lf", &n) 
#define pd(n) printf("%d", n)
#define pl(n) printf("%lld", n)
#define plf(n) printf("%0.9lf", n)
#define ps printf(" ")
#define pn printf("\n")


//ds helpers
#define f first
#define s second
#define pb push_back
#define pob pop_back
#define mp make_pair
#define sz(n) (ll)n.size()


//fast input and output when using cin,cout
#define fast ios_base::sync_with_stdio(false), cin.tie(0), cout.tie(0)
//some constants
const int INF = 1<<29;
#define MOD 1000000007
#define PI 3.14159265358979323846
#define EPS 0.0000000001
#define max_size 100005

//some helper functions
inline bool EQ(double a, double b) { return fabs(a-b) < 1e-9;}
inline ll lcm(ll a,ll b){ return (a*b)/(__gcd(a,b));}
#define mod(a) ((a)%MOD)
inline ll ciel(double a) { ll ret = a; if(a-ret>EPS) ret++; return ret; }
inline ll powf(ll n, ll p) {if(p==0)return 1; if(n<=1)return n;ll res = 1;while(p){if(p&1) res = mod(res*n);n = mod(n*n);p /= 2;} return res;}
inline bool ispoweroftwo(ll x){return (x && !(x&(x-1)));}

int ceiling_log2(unsigned long long x) {
  static const unsigned long long t[6] = {
    0xFFFFFFFF00000000ull,
    0x00000000FFFF0000ull,
    0x000000000000FF00ull,
    0x00000000000000F0ull,
    0x000000000000000Cull,
    0x0000000000000002ull
  };

  int y = (((x & (x - 1)) == 0) ? 0 : 1);
  int j = 32, i;

  for (i = 0; i < 6; i++) {
    int k = (((x & t[i]) == 0) ? 0 : j);
    y += k, x >>= k, j >>= 1;
  }

  return y;
}


int n,currnode,lft,rght,phase=0,replies=0,t,prevphase=-1,cnt=0;
int curruid;
map<int,pair<string,int>> numberipmap;
socklen_t clilen;
struct sockaddr_in cli_addr,server;



void hs(){
	//bind and receive here
	//open the message
	server.sin_family = AF_INET;
	int sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
      printf("Could not create socket");
    }
    server.sin_family = AF_INET;
    server.sin_port = htons(numberipmap[currnode].second);
    server.sin_addr.s_addr = INADDR_ANY;
    if (::bind(sock, (struct sockaddr *) &server, sizeof(server))<0) {
      printf("Problem binding\n");
      exit(0);
    }
    //cout<<"svrb";
    listen(sock,40);
    while(true){
    	//cout<<"hello\n";
    	int new_fd = accept(sock, (struct sockaddr *)&cli_addr, &clilen);
        if (new_fd < 0) {
              perror("accept");
              continue;
        }
        //cout<<"world\n";
        char buffero[1000];
        int zn = read(new_fd,buffero,sizeof buffero);
        if(zn<0){
            printf("error reading message\n");
            exit(0);
        }
        string msg = std::string(buffero);

		int sock1 =socket(AF_INET,SOCK_STREAM,0);
		if(sock1==-1){
			printf("Error in creating socket\n");
			exit(0);
		}
		string type= msg.substr(0,msg.find(','));
		msg = msg.substr(msg.find(',')+1);
		string dir = msg.substr(0,msg.find(','));			//breaking the message
		msg = msg.substr(msg.find(',')+1);
		string uid = msg.substr(0,msg.find(','));
		msg = msg.substr(msg.find(',')+1);
		int hopvalue = stoi(msg.substr(0,msg.find(',')));
		msg = msg.substr(msg.find(',')+1);
		int phvalue = stoi(msg);
		hopvalue--;
		if(stoi(type)==0){
			if(curruid>stoi(uid)){							//dropping the message and sending reply
				printf("Phase: %d uid is less than current uid so sending reply\n",phase);
				string newdir="";
				if(dir=="L"){
					newdir="R";
				}else{
					newdir="L";
				}
				msg = "-1,"+newdir + ","+uid + ","+to_string(phvalue-hopvalue)+","+to_string(phvalue);
				struct hostent *hp1;
				struct sockaddr_in server;
				server.sin_family = AF_INET;
				if(newdir=="L"){
					server.sin_port = htons(numberipmap[lft].second);
					if((hp1=gethostbyname((numberipmap[lft].first).c_str()))==0){
						printf("unknown host or invalid host %s\n",(numberipmap[lft].first).c_str());
						close(sock1);
						return;
					}

				}
				else{
					server.sin_port = htons(numberipmap[rght].second);
					if((hp1=gethostbyname((numberipmap[rght].first).c_str()))==0){
						printf("unknown host or invalid host %s\n",(numberipmap[lft].first).c_str());
						close(sock1);
						return;
					}
				}
				memcpy(&server.sin_addr.s_addr,hp1->h_addr,hp1->h_length);
				if(connect(sock1,(struct sockaddr*) &server,sizeof(server))<0){
					printf("connect Error to node%d from node%d\n",currnode,lft);
					close(sock1);
					return;
				}
				send(sock1,msg.c_str(),13,0);
				close(sock1);

			}else{
				printf("Phase : %dpassing message\n",phase);				//forwarding message
				struct hostent *hp1;
				struct sockaddr_in server;
				server.sin_family = AF_INET;
				if(hopvalue==0){			
					string newdir="";
					if(dir=="L"){
						newdir="R";							//send reply back that it is succesful
					}else{
						newdir="L";
					}
					if(newdir=="L"){
						server.sin_port = htons(numberipmap[lft].second);
						if((hp1=gethostbyname((numberipmap[lft].first).c_str()))==0){
							printf("unknown host or invalid host %s\n",(numberipmap[lft].first).c_str());
							close(sock1);
							return;
						}

					}
					else{
						server.sin_port = htons(numberipmap[rght].second);
						if((hp1=gethostbyname((numberipmap[rght].first).c_str()))==0){
							printf("unknown host or invalid host %s\n",(numberipmap[lft].first).c_str());
							close(sock1);
							return;
						}
					}
					memcpy(&server.sin_addr.s_addr,hp1->h_addr,hp1->h_length);
					if(connect(sock1,(struct sockaddr*) &server,sizeof(server))<0){
						printf("connect Error to node%d from node%d\n",currnode,lft);
						close(sock1);
						return;
					}
				
					msg = "1,"+newdir + ","+uid + ","+to_string(phvalue-hopvalue)+","+to_string(phvalue);											
					send(sock1,msg.c_str(),13,0);
				}else{
					if(dir=="L"){
						server.sin_port = htons(numberipmap[lft].second);
						if((hp1=gethostbyname((numberipmap[lft].first).c_str()))==0){
							printf("unknown host or invalid host %s\n",(numberipmap[lft].first).c_str());
							close(sock1);
							return;
						}

					}
					else{
						server.sin_port = htons(numberipmap[rght].second);
						if((hp1=gethostbyname((numberipmap[rght].first).c_str()))==0){
							printf("unknown host or invalid host %s\n",(numberipmap[lft].first).c_str());
							close(sock1);
							return;
						}
					}
					memcpy(&server.sin_addr.s_addr,hp1->h_addr,hp1->h_length);
					if(connect(sock1,(struct sockaddr*) &server,sizeof(server))<0){
						printf("connect Error to node%d from node%d\n",currnode,lft);
						close(sock1);
						return;
					}										//forward
					msg = "0,"+dir+","+uid+","+to_string(hopvalue)+","+to_string(phvalue);
					send(sock1,msg.c_str(),13,0);
					close(sock1);
				}
			}
		}else if(stoi(type)==1){
			struct hostent *hp1;
			struct sockaddr_in server;
			server.sin_family = AF_INET;
			if(hopvalue==0){
				cnt++;							//increase the cnt if cnt 2 increase the phase ,if phase is more than t it is leader
				if(cnt==2){
					cnt=0;
					phase++;
					if(phase>=t-1){
						printf("Phase %d I am the leader elected\n",phase-1);
						return;
					}
				}
			}else if(hopvalue>0){
										//forward it
				if(dir=="L"){
						server.sin_port = htons(numberipmap[lft].second);
						if((hp1=gethostbyname((numberipmap[lft].first).c_str()))==0){
							printf("unknown host or invalid host %s\n",(numberipmap[lft].first).c_str());
							close(sock1);
							return;
						}

					}
					else{
						server.sin_port = htons(numberipmap[rght].second);
						if((hp1=gethostbyname((numberipmap[rght].first).c_str()))==0){
							printf("unknown host or invalid host %s\n",(numberipmap[lft].first).c_str());
							close(sock1);
							return;
						}
					}
					memcpy(&server.sin_addr.s_addr,hp1->h_addr,hp1->h_length);
					if(connect(sock1,(struct sockaddr*) &server,sizeof(server))<0){
						printf("connect Error to node%d from node%d\n",currnode,lft);
						close(sock1);
						return;
					}
				msg = "1,"+dir+","+uid+","+to_string(hopvalue)+","+to_string(phvalue);
				send(sock1,msg.c_str(),13,0);
				close(sock1);
			}
		}else if(stoi(type)==-1){
			struct hostent *hp1;
			struct sockaddr_in server;
			server.sin_family = AF_INET;
			if(hopvalue==0){
				;
			}else if(hopvalue>0){
				;							//forward it
				if(dir=="L"){
						server.sin_port = htons(numberipmap[lft].second);
						if((hp1=gethostbyname((numberipmap[lft].first).c_str()))==0){
							printf("unknown host or invalid host %s\n",(numberipmap[lft].first).c_str());
							close(sock1);
							return;
						}

					}
					else{
						server.sin_port = htons(numberipmap[rght].second);
						if((hp1=gethostbyname((numberipmap[rght].first).c_str()))==0){
							printf("unknown host or invalid host %s\n",(numberipmap[lft].first).c_str());
							close(sock1);
							return;
						}
					}
					memcpy(&server.sin_addr.s_addr,hp1->h_addr,hp1->h_length);
					if(connect(sock1,(struct sockaddr*) &server,sizeof(server))<0){
						printf("connect Error to node%d from node%d\n",currnode,lft);
						close(sock1);
						return;
					}
				msg = "-1,"+dir+","+uid+","+to_string(hopvalue)+","+to_string(phvalue);
				send(sock1,msg.c_str(),13,0);
				close(sock1);
			}
		}
	}
}



/*sample input
5
10
1
192.168.0.107:5555
192.168.0.107:5556
192.168.0.107:5557
192.168.0.107:5558
192.168.0.107:5559


5
1
2
192.168.0.107:5555
192.168.0.107:5556
192.168.0.107:5557
192.168.0.107:5558
192.168.0.107:5559

5
8
3
192.168.0.107:5555
192.168.0.107:5556
192.168.0.107:5557
192.168.0.107:5558
192.168.0.107:5559

5
3
4
192.168.0.107:5555
192.168.0.107:5556
192.168.0.107:5557
192.168.0.107:5558
192.168.0.107:5559

5
7
0
192.168.0.107:5555
192.168.0.107:5556
192.168.0.107:5557
192.168.0.107:5558
192.168.0.107:5559
*/




int main(int argc, char const *argv[])
{
	/* code */
	sd(n);											/*number of node in the ring*/
	sd(curruid);
	sd(currnode);
	if(currnode>0 and currnode<n-1){
		lft = currnode-1;
		rght = currnode+1;
	}else if(currnode==0){								//find the lft and rght node of current node
		lft = n-1;
		rght=1;
	}else if(currnode==n-1){
		lft =n-2;
		rght = 0;
	}
	string s;
	FOR(i,0,n){
		cin>>s;											//get ip adresses of each node
		numberipmap[i] = {s.substr(0,s.find(':')),stoi(s.substr(s.find(':')+1))};
	}
	t = ceiling_log2(n);
	cout<<t<<endl;
	thread th ;
	th = thread(hs);
	sleep(10);
	while(phase<t-1){
		if(phase==prevphase) continue;
		prevphase = phase;
		int hopvalue = 1<<phase;
		string msgl = "0,L,"+to_string(curruid)+","+to_string(hopvalue)+","+to_string(hopvalue);
		string msgr = "0,R,"+to_string(curruid)+","+to_string(hopvalue)+","+to_string(hopvalue);
		int sock1 =socket(AF_INET,SOCK_STREAM,0);
		int sock2 =socket(AF_INET,SOCK_STREAM,0);
		if(sock1<0 or sock2<0){
			printf("Error in socket creation\n");
			close(sock1);
			close(sock2);
			return 0;
		}
		//send msgl to  lft node and msgr to rght node
		struct hostent *hp1;
		struct hostent *hp2;
		struct sockaddr_in server;
		server.sin_family = AF_INET;
		server.sin_port = htons(numberipmap[lft].second);
		
		if((hp1=gethostbyname((numberipmap[lft].first).c_str()))==0){
			printf("unknown host or invalid host %s\n",(numberipmap[lft].first).c_str());
			close(sock1);
			return 0;
		}
		memcpy(&server.sin_addr.s_addr,hp1->h_addr,hp1->h_length);
		if(connect(sock1,(struct sockaddr*) &server,sizeof(server))<0){
			printf("connect Error to node%d from node%d\n",currnode,lft);
			close(sock1);
			return 0 ;
		}
		send(sock1,msgl.c_str(),13,0);
		 server.sin_port = htons(numberipmap[rght].second);
		 if((hp2=gethostbyname((numberipmap[rght].first).c_str()))==0){
		 	printf("unknown host or invalid host %s\n",(numberipmap[rght].first).c_str());
		 	close(sock2);
		 	return 0;
		 }
		memcpy(&server.sin_addr.s_addr,hp2->h_addr,hp2->h_length);
		 if(connect(sock2,(struct sockaddr*) &server,sizeof(server))<0){
		 	printf("connect Error to node%d from node%d\n",currnode,rght);
		 	close(sock2);
		 	return 0;
		 }
		
		send(sock2,msgr.c_str(),13,0);	
		close(sock1);
		close(sock2);	
	}
	th.join();
	return 0;
}