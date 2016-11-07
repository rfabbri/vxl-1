#pragma warning(disable:4786)
#include "graph.h"
#include "functions.h"
#include "imgfunctions.h"
#include <set>
#include <deque>
#include <algorithm>
#include <cmath>
//using namespace std;



//
// graphm functions
//

graphm & graphm::operator=(const graphm &g){
    if(this!=&g){
        MakeGraph(g.n);
        e=g.e;
    }
    return *this;
};

graphm & graphm::operator=(graph<double> &g){
    vector<weighted_edge<double> >::iterator ei;
    vector<weighted_edge<double> > ge;
    int i,j;
    MakeGraph(g.e.size());
    e.SetDimension(n,n);
    e.Zero();
    g.GetUndirectedWEdgeList(ge);
    for (ei=ge.begin();ei!=ge.end();++ei){
        i=(*ei).i;
        j=(*ei).j;
        e(i,j)=e(j,i)=(*ei).value;
    }
    return *this;
};

void graphm::MakeGraph(int nv){
    n=nv;
    e.SetDimension(n,n);
    e.Zero();
};

int graphm::GetEdgeCount(){
    int i,j,count=0;
    for (i=0;i<n;i++)
        for (j=i+1;j<n;j++)
            if (e.Data(i,j)!=0)
                count++;
    return count;
}

//
// dirgraph functions
//

void dirgraph::MakeGraph(int nv){
    List L;
    int i;
    n=nv;
    L.clear();
    Adj.clear();
    for (i=0;i<nv;i++)
        Adj.push_back(L);
}

void dirgraph::ResizeGraph(int nv){
    List L;
    n=nv;
    L.clear();
    if (n<Adj.size()){
        while (n<Adj.size()){
            Adj.pop_back();
        }
    }
    else{
        while(n>Adj.size()){
            Adj.push_back(L);
        }
    }
}

bool dirgraph::Edge(int u, int v){    
    //returns true if there is an edge from u to v
    int i;
    for (i=0;i<Adj[u].size();i++){
        if (Adj[u][i]==v)
            return true;
    }
    return false;
}

void dirgraph::InEdges(int u, List &l){
    int i,j,n1;
    l.clear();
    for (i=0;i<n;i++){
        n1=Adj[i].size();
        for (j=0;j<n1;j++){
            if (u==Adj[i][j]){
                l.push_back(i);
            }
        }
    }
}

int dirgraph::DFS_Visit(int u){
    int i,n1=Adj[u].size(),v;
    col[u]=1;
    time++;
    d[u]=time;
    for (i=0;i<n1;i++){
        v=Adj[u][i];
        if (col[v]==0){
            p[v]=u;
            DFS_Visit(v);
        }
    }
    col[u]=2;
    time++;
    f[u]=time;
    rez.push_back(u);
    return time;
}

bool dirgraph::IsAcyclic(){
    int u,n1,i,v;
    List in;
    in.clear();
    for (u=0;u<n;u++){
        in.push_back (0);
    }
    for (u=0;u<n;u++){
        n1=Adj[u].size();
        if (n1=0) return true;
        for (i=0;i<n1;i++){
            v=Adj[u][i];
            in[v]=1;            //mark all vertices that have incomming edges
        }
    }
    for (u=0;u<n;u++){
        if (in[u]==0) return true;
    }
    return false;
}

int    dirgraph::rankindex(int u){
    int i;
    for (i=0;i<n;i++){
        if (rank[i]==u)
            return i;
    }
    return -1;
}

bool dirgraph::TopSort(){
    //runs DFS to sort vertices in the order of the arrows, return false if not acyclic
    int u,n1,i,v;
    col.clear();p.clear();
    rank.clear();rez.clear();
    for (u=0;u<n;u++){
        col.push_back(0);
        p.push_back(-1);
        d.push_back(0);
        f.push_back(0);
    }
    time=0;
    for (u=0;u<n;u++){
        if (col[u]==0){
            DFS_Visit(u);
        }
    }
    for (u=0;u<n;u++){
        rank.push_back(rez[n-1-u]);
    }

    for (u=0;u<n;u++){
        n1=Adj[u].size();
        for (i=0;i<n1;i++){
            v=Adj[u][i];
            if (rank[u]>rank[v])
                return false;
        }
    }

    return true;
}

void dirgraph::printorder(char *text){
    char st[255];
    strcpy(text,"graph rank order: ");
    for(int i=0;i<rank.size();i++){
        if (i<rank.size()-1)
            sprintf(st,"%d,",rank[i]);
        else
            sprintf(st,"%d",rank[i]);
        strcat(text,st);
    }
}

void dirgraph::Save(char *filename){
    int i,u;
    FILE *f=fopen(filename,"w");
    for (u=0;u<n;u++){
        for (i=0;i<Adj[u].size();i++){
            if (i<Adj[u].size()-1)
                fprintf(f,"%d,",Adj[u][i]);
            else
                fprintf(f,"%d",Adj[u][i]);
        }
        fprintf(f,"\n");
    }
    fclose(f);
}

void dirgraph::Load(char *filename){
    FILE *f = fopen(filename,"r");
    Matrix<float> M;
    List L;
    int i,k;
    char buff[10000],line[10000];
    if (f==NULL) return ;
    Adj.clear();
    do{     //each Adj. line
        if (fgets(buff,10000,f)!=NULL){ 
            L.clear();
            strcpy(line,buff);
            lineparse(line,10000,',',M);
            if (M.cols()>0){
                for (i=0;i<M.cols();i++){
                    k=(int)M(0,i);
                    L.push_back(k);
                    if (n<k+1) n=k+1;
                }
            }
            Adj.push_back(L);
        }
    }while(!feof(f));
    fclose(f);
    if (n<Adj.size()) n=Adj.size();
    TopSort();
}

/*
void SampleGraph(graph<double> &g,graph<double> &g1){
    //sample each edge of a weighted graph based on the probabilities given by its weight
    //put the new graph in g1
    double d;
    dEdgeList::iterator ei;
    dEdgeList el;
    dAdjacencyList::iterator ai;
    int i;
    double w;

    g1.MakeGraph(g.e.size());
    g1.e.clear();
    for (ai=g.e.begin();ai!=g.e.end();++ai){
        el.clear();
        for (ei=(*ai).begin();ei!=(*ai).end();++ei){
            d=randdbl(1);
//            w=((*ei).second).first;
            w=(*ei).second;
            if (d<w){
                i=(*ei).first;
                el.insert(dEdgeList::value_type(i,w));
            }
        }
        g1.e.push_back(el);
    }
}
void SampleGraph(graph<double> &g,graph<double> &g1,double power){
    //sample each edge of a weighted graph based on the probabilities given by its weight
    //put the new graph in g1
    double d;
    dEdgeList::iterator ei;
    dEdgeList el;
    dAdjacencyList::iterator ai;

    g1.e.clear();
    for (ai=g.e.begin();ai!=g.e.end();++ai){
        el.clear();
        for (ei=(*ai).begin();ei!=(*ai).end();++ei){
            d=randdbl(1);
            if (d<pow((*ei).second,power)){
                el.insert(*ei);
            }
        }
        g1.e.push_back(el);
    }
}
*/

void SampleGraph(graphm &gm, graph<double> &g){
    //sample each edge of a weighted graph based on the probabilities given by its weight
    //put the new graph in g1
    int i,j,n=gm.n;
    double d;
    g.MakeGraph(n);
    for(i=0;i<n;i++){
        for(j=i+1;j<n;j++){
            d=gm.e(i,j);
            if (d>0){
                if (SampleProb(d)){
                    g.InsertEdge(i,j,1);
                }
            }
        }
    }
}
/*
void ThreshGraph(graph<double> &g,graph<double> &g1,double thresh){
    //kill edges below thresh
    dEdgeList::iterator ei;
    dEdgeList el;
    dAdjacencyList::iterator ai;

    g1.e.clear();
    for (ai=g.e.begin();ai!=g.e.end();++ai){
        el.clear();
        for (ei=(*ai).begin();ei!=(*ai).end();++ei){
            if (thresh<(*ei).second){
                el.insert(*ei);
            }
        }
        g1.e.push_back(el);
    }
}
*/

void findMinimumSpanningTree(graph<double> &g, graphuf<double> &g1){
    //finds MST for graph g, stores it in g1
    int n=g.e.size(),u,v;
    vector<weighted_edge<double> >::iterator ei;
    vector<weighted_edge<double> > ge;
    bool first=true;
    g1.MakeGraph(n);
    g.GetUndirectedWEdgeList(ge);
    sort(ge.begin(),ge.end());
    for(ei=ge.begin();ei!=ge.end();++ei){
        u=(*ei).i;v=(*ei).j;
        if (g1.findset(u)!=g1.findset(v)){
            g1.InsertEdge((*ei).i,(*ei).j,(*ei).value);
            g1.setunion(u,v);
        }
    }
}
/*
void MakeProbGraph(graph<double> &g, graph<double> &g1, double Temp){
    dEdgeList::iterator ei;
    dEdgeList el;
    dAdjacencyList::iterator ai;
    double d;
    int i;

    g1.e.clear();
    for (ai=g.e.begin();ai!=g.e.end();++ai){
        el.clear();
        for (ei=(*ai).begin();ei!=(*ai).end();++ei){
            d=exp(-(*ei).second/Temp);
            i=(*ei).first;
            el.insert(dEdgeList::value_type(i,d));
        }
        g1.e.push_back(el);
    }
}
*/
void MakeProbGraph(graph<double> &g, graphm &g1, double Temp){
    vector<weighted_edge<double> >::iterator ei;
    vector<weighted_edge<double> > ge;
    double d;
    g.GetUndirectedWEdgeList(ge);
    g1.MakeGraph(g.e.size());
    for(ei=ge.begin();ei!=ge.end();++ei){
        d=(*ei).value;
        g1.e((*ei).i,(*ei).j)=g1.e((*ei).j,(*ei).i)=exp(-d/Temp);
    }
}

void MakeProbGraph(graphm &g, graph<double> &g1, double Temp){
    int i,j,n=g.n;
    double d;
    g1.MakeGraph(n);
    for(i=0;i<n;i++){
        for(j=i+1;j<n;j++){
            d=g.e(i,j);
            if (d>0){
                g1.InsertEdge(i,j,exp(-d/Temp));
            }
        }
    }
}

void MakeProbGraph(graphm &g, graphm &g1, double Temp){
    int i,j,n=g.n;
    double d;
    g1.MakeGraph(n);
    for(i=0;i<n;i++){
        for(j=i+1;j<n;j++){
            d=g.e(i,j);
            if (d>0){
                g1.e(i,j)=g1.e(j,i)=exp(-d/Temp);
            }
        }
    }
}
/*
int find(vector<int> L, int val){
    //finds val in vector L returns -1 if not found, else the position of first found
    int i,n=L.size();
    for (i=0;i<n;i++)
        if (L[i]==val) return i;
    return -1;
}
*/
void BreadthFirstSearch::DoSearch(int s, vector<int> &d, vector<int> &pred){
    int u,v,n=g->size();
    vector<int> color;    //0=black,128=gray,255=white
    deque<int> Q;
    dEdgeList::iterator mi;
    color.clear();
    color.assign(n,255);
    d.assign(n,1000000);
    pred.assign(n,-1);
    color[s]=128;
    d[s]=0;
    Q.clear();
    Q.push_back(s);
    while(Q.size()!=0){
        u=*Q.begin();
        for (mi=g->e[u].begin();mi!=g->e[u].end();++mi){
            v=(*mi).first;
            if (color[v]==255){
                color[v]=128;
                d[v]=d[u]+1;
                pred[v]=u;
                Q.push_back(v);
            }
        }
        Q.pop_front();
        color[u]=0;
    }
}

void BreadthFirstSearch::GetPath(vector<int> &pred,int d,vector<int> &path){
    int u;
    path.clear();
    u=d;
    do{
        path.push_back(u);
        u=pred[u];
    }
    while (u!=-1);
}
void BreadthFirstSearch::GetLongestPath(vector<int> &path){
    vector<int> l,pred;
    int i,s,n=g->size(),max=0,dmax;
    for (s=0;s<n;s++){
        DoSearch(s,l,pred);
        dmax=-1;
        for (i=0;i<n;i++){
            if (l[i]>max){
                max=l[i];
                dmax=i;
            }
        }
        if (dmax>=0){
            GetPath(pred,dmax,path);
        }
    }
}

void print(char *filename, vector<weighted_edge<double> > &m){
    int i,n=m.size();
    weighted_edge<double> *e;
    if (n==0)
        return;
    FILE *f=fopen(filename, "w" );
    for (i=0;i<n;i++){
        e=&m[i];
        fprintf(f,"%d,%d,%1.4f\r\n",e->i,e->j,e->value);
    }
    fclose(f);
}




