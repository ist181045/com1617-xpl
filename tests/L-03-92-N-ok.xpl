int g(int i)
int f(int i) = 9 { if (i == 0) f = 0; else f = g(i-1) + i; }
int g(int i) = 9 { if (i == 0) g = 0; else g = f(i-1) + i; }
public int xpl() = 0 { f(3)! }

