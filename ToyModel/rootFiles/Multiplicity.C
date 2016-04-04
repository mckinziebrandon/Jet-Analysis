{

  gROOT->SetStyle("Plain");

  // Plot: p7937_d1x1y3
  double p7937_d1x1y1_xval[] = { 2.5, 7.5, 15.0, 25.0, 35.0, 45.0, 55.0, 65.0, 75.0 };
  double p7937_d1x1y1_xerrminus[] = { 2.5, 2.5, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0 };
  double p7937_d1x1y1_xerrplus[] = { 2.5, 2.5, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0 };
  double p7937_d1x1y1_yval[] = { 1601.0, 1294.0, 966.0, 649.0, 426.0, 261.0, 149.0, 76.0, 35.0 };
  double p7937_d1x1y1_yerrminus[] = { 60.0, 49.0, 37.0, 23.0, 15.0, 9.0, 6.0, 4.0, 2.0 };
  double p7937_d1x1y1_yerrplus[] = { 60.0, 49.0, 37.0, 23.0, 15.0, 9.0, 6.0, 4.0, 2.0 };
  double p7937_d1x1y1_ystatminus[] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
  double p7937_d1x1y1_ystatplus[] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
  int p7937_d1x1y1_numpoints = 9;
  p7937_d1x1y1 = TGraphAsymmErrors(p7937_d1x1y1_numpoints, p7937_d1x1y1_xval, p7937_d1x1y1_yval, p7937_d1x1y1_xerrminus, p7937_d1x1y1_xerrplus, p7937_d1x1y1_yerrminus, p7937_d1x1y1_yerrplus);
  p7937_d1x1y1.SetName("/HepData/7937/d1x1y1");
  p7937_d1x1y1.SetTitle("/HepData/7937/d1x1y1");
  p7937_d1x1y1.Draw("AP");

  TFile* f= new TFile("Multiplicity.root", "RECREATE");
  f->cd();
  p7937_d1x1y1.Write();
  f->Close();

  double p7937_d1x1y2_xval[] = { 2.5, 7.5, 15.0, 25.0, 35.0, 45.0, 55.0, 65.0, 75.0 };
  double p7937_d1x1y2_xerrminus[] = { 2.5, 2.5, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0 };
  double p7937_d1x1y2_xerrplus[] = { 2.5, 2.5, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0 };
  double p7937_d1x1y2_yval[] = { 382.8, 329.7, 260.5, 186.4, 128.9, 85.0, 52.8, 30.0, 15.8 };
  double p7937_d1x1y2_yerrminus[] = { 3.1, 4.6, 4.4, 3.9, 3.3, 2.6, 2.0, 1.3, 0.6 };
  double p7937_d1x1y2_yerrplus[] = { 3.1, 4.6, 4.4, 3.9, 3.3, 2.6, 2.0, 1.3, 0.6 };
  double p7937_d1x1y2_ystatminus[] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
  double p7937_d1x1y2_ystatplus[] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
  int p7937_d1x1y2_numpoints = 9;
  p7937_d1x1y2 = TGraphAsymmErrors(p7937_d1x1y2_numpoints, p7937_d1x1y2_xval, p7937_d1x1y2_yval, p7937_d1x1y2_xerrminus, p7937_d1x1y2_xerrplus, p7937_d1x1y2_yerrminus, p7937_d1x1y2_yerrplus);
  p7937_d1x1y2.SetName("/HepData/7937/d1x1y2");
  p7937_d1x1y2.SetTitle("/HepData/7937/d1x1y2");

  double p7937_d1x1y3_xval[] = { 2.5, 7.5, 15.0, 25.0, 35.0, 45.0, 55.0, 65.0, 75.0 };
  double p7937_d1x1y3_xerrminus[] = { 2.5, 2.5, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0 };
  double p7937_d1x1y3_xerrplus[] = { 2.5, 2.5, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0 };
  double p7937_d1x1y3_yval[] = { 8.4, 7.9, 7.4, 7.0, 6.6, 6.1, 5.7, 5.1, 4.4 };
  double p7937_d1x1y3_yerrminus[] = { 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.4 };
  double p7937_d1x1y3_yerrplus[] = { 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.4 };
  double p7937_d1x1y3_ystatminus[] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
  double p7937_d1x1y3_ystatplus[] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
  int p7937_d1x1y3_numpoints = 9;
  p7937_d1x1y3 = TGraphAsymmErrors(p7937_d1x1y3_numpoints, p7937_d1x1y3_xval, p7937_d1x1y3_yval, p7937_d1x1y3_xerrminus, p7937_d1x1y3_xerrplus, p7937_d1x1y3_yerrminus, p7937_d1x1y3_yerrplus);
  p7937_d1x1y3.SetName("/HepData/7937/d1x1y3");
  p7937_d1x1y3.SetTitle("/HepData/7937/d1x1y3");

}
