#include<bits/stdc++.h>
#define int long long
using namespace std;
const int N=1e9;
int vis[N];
int bfs(vector<int> a,vector<int> b,int n){
    queue<int> q;
    int id=0;
    int ans=0;
    q.push(id);
    while(!q.empty()){
        if(id==n-1){return ans;}
        ans++;
        id=q.front();
        vis[id]=1;
        q.pop();
        int sign=min(b[id],n-id-1);
        for (int i = 0; i < sign; ++i) {
            int nid=id-i-1;
            if(vis[nid]==1){ continue;}
            vis[nid]=1;
            nid-=a[nid];
            q.push(nid);
        }
    }
    return ans;
}
signed main(){
    for (int & vi : vis) {
        vi=0;
    }
    int n;
    cin>>n;
    vector<int> a(n),b(n);
    for (int i = 0; i < n; ++i) {
        cin>>a[i];
    }
    for (int i = 0; i < n; ++i) {
        cin>>b[i];
    }
    int ans=bfs(a,b,n);
    cout<<ans;
    return 0;
}
