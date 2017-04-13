public real main([real] p) { main = p[0]; }
public int xpl() = 0 {
  [real] p = null;
  p = [3];
  p[0] = 3.1;
  p[1] = 2.1;
  p[2] = 6.1E1;
  main(p[2]?)!
}

