#include <iostream>
#include <bitset>
#include <queue>
#include <vector>

using namespace std;

namespace graphs
{
	struct edge
	{
		int to, capacity, reverse, flow, cost;
		edge (int a = 0, int b = 0, int c = 0, int d = 0): to(a), capacity(b), reverse(c), flow(0), cost(d) {}
	};
	
	typedef vector < vector < edge > > graph;
}

using namespace graphs;

namespace flows
{
	const int INF = 1e9;
	
	class ford_fulkerson
	{
	public:
		static const int N = 1e3;
		bitset < N > vis;
		graph g;
		int source, sink, ans = 0;
		int dfs (int v, int mn)
		{
			if (v == sink)
				return mn;
			vis[v] = true;
			for (auto &it: g[v])
				if (!vis[it.to] && it.flow < it.capacity)
				{
					int delta = dfs(it.to, min(mn, it.capacity-it.flow));
					if (delta > 0)
					{
						it.flow += delta;
						g[it.to][it.reverse].flow -= delta;
						return delta;
					}
				}
			return 0;
		}
		ford_fulkerson (const graph &g, int source, int sink): g(g), source(source), sink(sink) {}
		int evaluate ()
		{
			if (ans == 0)
			{
				int tmp = 0;
				do
				{
					vis.reset();
					tmp = dfs(source, INF);
					ans += tmp;
					
				} while (tmp != 0);
			}
			return ans;
		}
	};
	
	class edmonds_carp
	{
	public:
		graph g;
		int source, sink, ans = 0;
		edmonds_carp (const graph &g, int source, int sink): g(g), source(source), sink(sink) {}
		int evaluate ()
		{
			if (ans == 0)
			{
				int tmp;
				do
				{
					vector < int > mn(g.size(), 0), p(g.size(), 0);
					mn[source] = INF;
					queue < int > que;
					que.push(source);
					while (!que.empty())
					{
						int v = que.front();
						que.pop();
						for (auto &it: g[v])
							if (mn[it.to] == 0 && it.flow < it.capacity)
							{
								mn[it.to] = min(mn[v], it.capacity-it.flow);
								p[it.to] = it.reverse;
								que.push(it.to);
							}
					}
					tmp = mn[sink];
					if (tmp != 0)
					{
						int v = sink;
						while (v != source)
						{
							g[v][p[v]].flow -= tmp;
							auto edge = g[v][p[v]];
							g[edge.to][edge.reverse].flow += tmp;
							v = edge.to;
						}
					}
					ans += tmp;
				} while (tmp != 0);
			}
			return ans;
		}
	};
	
	class dinic
	{
	public:
		graph g;
		vector < int > p, d;
		int source, sink, ans = 0;
		bool bfs ()
		{
			fill(d.begin(), d.end(), INF);
			d[source] = 0;
			queue < int > que;
			que.push(source);
			while (!que.empty())
			{
				int v = que.front();
				que.pop();
				for (auto it: g[v])
					if (d[it.to] == INF && it.flow < it.capacity)
					{
						d[it.to] = d[v]+1;
						que.push(it.to);
					}
			}
			return d[sink] != INF;
		}
		int dfs (int v, int mn)
		{
			if (v == sink)
				return mn;
			while (p[v] != g[v].size())
			{
				auto it = g[v][p[v]];
				if (d[it.to] == d[v]+1 && it.flow < it.capacity)
				{
					int delta = dfs(it.to, min(mn, it.capacity-it.flow));
					if (delta > 0)
					{
						g[v][p[v]].flow += delta;
						g[it.to][it.reverse].flow -= delta;
						return delta;
					}
				}
				++p[v];
			}
			return 0;
		}
		dinic (const graph &g, int source, int sink): g(g), source(source), sink(sink)
		{
			p.resize(g.size(), 0);
			d.resize(g.size(), INF);
		}
		int evaluate ()
		{
			if (ans == 0)
				while (bfs())
				{
					fill(p.begin(), p.end(), 0);
					int tmp;
					do
					{
						tmp = dfs(source, INF);
						ans += tmp;
					} while (tmp);
				}
			return ans;
		}
	};
}

using namespace flows;

namespace matching
{
	class kuhn
	{
	public:
		static const int N = 1e5;
		bitset < N > vis;
		graph g;
		int n, ans = 0;
		int *match;
		kuhn (const graph &g, int n): g(g), n(n)
		{
			match = new int[N];
			for (int i = 0; i <= g.size(); ++i)
				match[i] = 0;
		}
		~kuhn ()
		{
			delete match;
		}
		bool dfs (int v)
		{
			if (vis[v])
				return false;
			vis[v] = true;
			if (v > n)
			{
				if (!match[v])
					return true;
				return dfs(match[v]);
			}
			else
				for (auto it: g[v])
					if (it.to != match[v] && dfs(it.to))
					{
						match[v] = it.to;
						match[it.to] = v;
						return true;
					}
			return false;
		}
		int evaluate ()
		{
			if (ans == 0)
			{
				bool changed;
				do
				{
					changed = false;
					vis.reset();
					for (int i = 1; i <= n; ++i)
						if (!match[i] && dfs(i))
						{
							changed = true;
							++ans;
						}
				} while (changed);
			}
			return ans;
		}
	};
}

using namespace matching;

int main ()
{
	ios::sync_with_stdio(0);
	int n, m, k;
	cin >> n >> m >> k;
	graph g;
	if (true)
	{
		g.resize(n+m+1);
		while (k--)
		{
			int from, to;
			cin >> from >> to;
			g[from].push_back(edge(to+n));
			g[to+n].push_back(edge(from));
		}
		kuhn mm(g, n);
		cout << mm.evaluate() << endl;
		for (int i = 1; i <= n; ++i)
			if (mm.match[i])
				cout << i << ' ' << mm.match[i]-n << endl;
	}
	else
	{
		g.resize(n+m+3);
		while (k--)
		{
			int from, to;
			cin >> from >> to;
			g[from+1].push_back(edge(to+n+1, 1, g[to+n+1].size()));
			g[to+n+1].push_back(edge(from+1, 0, g[from+1].size()-1));
		}
		for (int i = 2; i <= n+1; ++i)
		{
			g[1].push_back(edge(i, 1, g[i].size()));
			g[i].push_back(edge(1, 0, g[1].size()-1));
		}
		for (int i = n+2; i <= n+m+1; ++i)
		{
			g[i].push_back(edge(n+m+2, 1, g[n+m+2].size()));
			g[n+m+2].push_back(edge(i, 0, g[i].size()-1));
		}
		dinic mm(g, 1, n+m+2);
		cout << mm.evaluate() << endl;
		for (int i = 2; i <= n+1; ++i)
			for (auto it: mm.g[i])
				if (it.flow > 0)
					cout << i-1 << ' ' << it.to-n-1 << endl;
	}
	return 0;
}