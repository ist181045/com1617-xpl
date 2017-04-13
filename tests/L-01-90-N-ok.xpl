int f(int i) {
  if (i == 0) f = 0; else { f = f(i-1) + i; return }
  f = 9;
}
public int xpl() = 0 { f(3)! }

