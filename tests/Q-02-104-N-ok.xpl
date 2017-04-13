public [real] main([real] p) { main = 1 + p; }
public int xpl() = 0 {
  [real] p = null;
  [real] q = null;
  p = [3];
  p[0] = 3.1;
  p[1] = 2.1;
  p[2] = 6.1E1;
  q = main(main(p));
  q[0]!
}

