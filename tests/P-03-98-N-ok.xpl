[int] f;
public int xpl() = 0 {
  int n = 4;
  [int] f = null;
  f = [n];
  { int i; sweep+(i:0:n-1) f[i] = 2; }
  { int j; sweep+(j:0:n-1) f[j]! }
  "\n"!
  { int k; sweep+(k:0:n-1) f[k] = 6/f[k]; }
  { int l; sweep+(l:0:n-1) f[l]! }
}

