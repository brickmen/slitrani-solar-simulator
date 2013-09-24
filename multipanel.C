{
  gROOT->Reset();
  char *name    = "world beta";
  char *listing = "world beta";
  char *upcom   = "Simple Solar Panel in SLitrani with Real Sun Simulation";
  char *downcom = "In a more realistic world";
  gROOT->ProcessLine(".x InitSLitrani.C(5,name,listing,upcom,downcom,kFALSE,kTRUE,kFALSE)");
  //
  // arguments of InitSLitrani:
  //
  // size     size of main canvas of TwoPadDisplay [ 1 <= size <= 8 ]
  // name     name of the produced ROOT file (without ".root")
  // listing  name of the listing file (without ".lis")
  // upcom    name of the comment appearing on top    left of canvas
  // downcom  name of the comment appearing on bottom left of canvas
  // otherseq true if one wants an other sequence of random numbers at each new launching
  // indebug  true if one wants to run Litrani in debug mode
  // WithDate if true, appends date and time to name of root file and name of listing
 
  const char *comTL = "simple solar panel";
  const Color_t  TotAbsColor    =  1;
  const Color_t  PanelColor     =  2;
  const Color_t  FibreColor     =  5;
  const Double_t IrrA           = 0.0; //Value used for A   when irrelevant !
  const Double_t IrrZ           = 0.0; //Value used for Z   when irrelevant !
  const Double_t IrrRho         = 0.0; //Value used for Rho when irrelevant !
  Int_t mediumindex = 1;
  gCleanOut->fMaxInfo = 1000;
  Short_t ns = -1;

  ns = 1;
  //____________________________________________________________________________
  //____________________________________________________________________________
  //
  // Building the geometry
  //____________________________________________________________________________
  //____________________________________________________________________________
  //
  TGeoManager *geom = new TGeoManager("setup","Solar Panel of new Litrani");
  //____________________________________________________________________________
  //
  // Materials and media
  //____________________________________________________________________________
  //
  // (1) Vacuum for "TOP"
  //
  //  We define it as a medium where photons CAN travel, although according to the above
  // description of the setup, it is impossible for a photon to reach it
  //
  TGeoMaterial *vacuum_mat = new TGeoMaterial("Vacuum",IrrA,IrrZ,IrrRho);
  TLitMedium *vacuum = new TLitMedium("Vacuum",mediumindex++,vacuum_mat,kFALSE,1.0,100000.0);
  vacuum->IsIsotropic(1.0);
  //
  // Load the periodic table for elements
  //
  TGeoElementTable *table = gGeoManager->GetElementTable();
  //
  // (2) Silicon for the solar panel
  //
  TGeoElement *Si = table->FindElement("SILICON");
  TGeoMaterial *silicon_mat = new TGeoMaterial("Silicon",Si,2.33);
  TLitMedium *silicon = new TLitMedium("Silicion",mediumindex++,silicon_mat,kTRUE,1.0,
   0.002);
  silicon->IsIsotropic(1.0);
  //
  // (3) Plastic is used to simulate sun source
  //
   TGeoMaterial *plastic_mat = new TGeoMaterial("Plastic",IrrA,IrrZ,IrrRho);
  //  Since this medium is intended for a fibre, we put an absorption length of 0.
  // See paragraph "Simulating an optical fibre" of the class description of TLitSpontan
  // to understand why.
  TLitMedium *plastic = new TLitMedium("Plastic",mediumindex++,plastic_mat,kFALSE,1.0,0.0);
  plastic->IsIsotropic(1.5);
  //____________________________________________________________________________
  //
  // Wrappings
  //____________________________________________________________________________
  //
  //
  //  (4) Definition of "TotAbsorbing": a totally absorbing wrapping,
  //        necessary to stop photons exiting world
  //
  TGeoMaterial *totabs_mat = new TGeoMaterial("TotAbsorbing",IrrA,IrrZ,IrrRho);
  TLitMedium *totabsorbing = new TLitMedium("TotAbsorbing",mediumindex++,totabs_mat,-1.0);
  totabsorbing->SetAsWrapping(0.0,0.0,1.0,1.0,1.0);
  totabsorbing->IsIsotropic(1.0); //Only useful if last arg of constructor changed to +1.0
  //____________________________________________________________________________
  //
  // Dimensions
  //____________________________________________________________________________
  //
  // Dimensions of TOP
  //
  const Double_t top_rmin    = 0.0;
  const Double_t top_rmax    = 10.51;
  const Double_t top_thmin   = 0.0;
  const Double_t top_thmax   = 110.0;
  const Double_t top_phmin   = 0.0;
  const Double_t top_phmax   = 360.0;
  //
  // Dimensions of Panel
  //
  const Double_t panel_dx = 2.0;
  const Double_t panel_dy = 4.0;
  const Double_t panel_dz = 0.1;
  //
  // Dimensions of Fibre (Sun Source)
  //
  Double_t dist          = (top_rmax - 0.51);
  Double_t delta         = (0.009308);
  Double_t fib_r         = 10*(delta * 0.5 * dist) ;
  const Double_t fib_dz  = 0.1;
  //____________________________________________________________________________
  //
  // Positionning (translations)
  //____________________________________________________________________________
  //
  // Positioning the panel
  //
  Double_t t_panel_tot_z = (2*panel_dz+0.02);
  TGeoTranslation *panel_pstn1 = new TGeoTranslation("panel_pstn1",-4,0.0,t_panel_tot_z);
  TGeoTranslation *panel_pstn2 = new TGeoTranslation("panel_pstn2",0,0.0,t_panel_tot_z);
  TGeoTranslation *panel_pstn3 = new TGeoTranslation("panel_pstn3",4,0.0,t_panel_tot_z);
  //
  //Positioning the Laser
  //
  Double_t t_laser_raduis = top_rmax - 0.51;
  Double_t phi,theta,psi;
  TGeoRotation rlaser;
  rlaser.SetAngles( 90.0 , 90.0 , 0.0);
  TGeoTranslation tlaser(t_laser_raduis,0.0,0.0);
  TGeoCombiTrans *claser = new TGeoCombiTrans(tlaser,rlaser);
  TGeoHMatrix *phlaser = new TGeoHMatrix(*claser);


  //____________________________________________________________________________
  //
  // Volumes and nodes
  //____________________________________________________________________________
  //
  //
  // top box containing everything and defining the WCS, world coordinate system
  //   
  TGeoVolume *top = geom->MakeSphere("TOP",totabsorbing,top_rmin,top_rmax,top_thmin,top_thmax, top_phmin, top_phmax);
  geom->SetTopVolume(top);
  TGeoVolume *tot = geom->MakeSphere("TOT",vacuum,top_rmin,(top_rmax-0.01),top_thmin,top_thmax,top_phmin,top_phmax);
  top->AddNode(tot,1,new TGeoTranslation("totpstn",0.0,0.0,0.005));
  TGeoVolume *tot_disc = geom->MakeTube("TOT_DISC",totabsorbing,0.0,(top_rmax-0.6),0.0001);
  tot->AddNode(tot_disc,1,new TGeoTranslation("totpstn",0.0,0.0,0.0));
  //
  TGeoVolume *panel = geom->MakeBox("PANEL",silicon,panel_dx,panel_dy,panel_dz);
  
  //  apd being a detector, a call to TLitVolume is necessary. All characteristics of the
  // APD will then be given by a call to TLitVolume::SetAPD().
  TLitVolume *lit_panel = new TLitVolume(panel);
  lit_panel->SetDetector(kFALSE, "", 180.0, 270.);
  tot->AddNode(panel,1,panel_pstn1);
  tot->AddNode(panel,1,panel_pstn2);
  tot->AddNode(panel,1,panel_pstn3);
  
  //
  // TGeoTube fib made of plastic
  //
  TGeoVolume *fib = geom->MakeTube("FIB",plastic,0.0,fib_r,fib_dz);
  tot->AddNode(fib,1, phlaser);
  //  We have to associate a TLitVolume to this TGeoVolume, since it is a source of photons
  TLitVolume *lit_fib = new TLitVolume(fib);
  TVector3 emisdir(0.0,0.0,-1.0); // axis around which photons are emitted
  TVector3 facedir(0.0,0.0,1.0); // axis to follow to reach face of emission
  TVector3 source(0.0,0.0,0.0);  // irrelevant
  lit_fib->SetEmission(flat,180.0,emisdir,"",kFALSE,source,kTRUE,facedir);
  //
  geom->CloseGeometry();
  geom->CheckOverlaps(0.01);
  //____________________________________________________________________________
  //____________________________________________________________________________
  //
  // End of Building the geometry
  //____________________________________________________________________________
  //____________________________________________________________________________
  //
  //____________________________________________________________________________
  //
  // Colors and drawing
  //____________________________________________________________________________
  //
  top->SetVisibility(kFALSE);
  top->SetVisContainers();
  top->SetLineColor(1);
  top->SetLineWidth(1);
  tot->SetVisibility(kTRUE);
  panel->SetVisibility(kTRUE);
  panel->SetLineColor(PanelColor);
  panel->SetLineWidth(1);
  fib->SetVisibility(kTRUE);
  fib->SetLineColor(FibreColor);
  fib->SetLineWidth(1);
  gGeoManager->SetTopVisible(1);
  gGeoManager->SetVisLevel(4);
  //____________________________________________________________________________
  //____________________________________________________________________________
  //
  // Preparing Solar Spectrum
  //____________________________________________________________________________
  //____________________________________________________________________________
  //
  //  Read data from SMARTS output and load into a ROOT histogram for processing
  //
  // Modified from Rene Brun's basic.C tutorial
  //
   char *finname = "ASTMG173.csv"; //Name of Smarts file in Use
   //
   cout << "inputname     : " << finname << endl;
   //
   TString dir = gSystem->UnixPathName(gInterpreter->GetCurrentMacroName());
   dir.ReplaceAll("fileinput.C","");
   dir.ReplaceAll("/./","/");
   ifstream in;
   in.open(Form( finname ,dir.Data()));
   //
   //Setup Recieving File and Folders
   TFile *f = new TFile("finputoutput.root","RECREATE");
   TH1F *h1 = new TH1F("h1","basis",2002,280.0,4000.0);
   TTree *t = new TTree("ntuple","data from csv file");
   //
   //Built In Routine to read file into a tree
   t.ReadFile(finname,
             "Wavelength:Extra_Terrestial_Radiation:Global_Tilt:Direct_and_Circumsolar");
   /*This reads to command line the data extracted from the file
   //BELOW, OPTIONAL:
   //
   Double_t Wavelength, Extra_Terrestial_Radiation, Global_Tilt, Direct_and_Circumsolar;
   t.SetBranchAddress("Wavelength", &Wavelength);
   t.SetBranchAddress("Extra_Terrestial_Radiation", &Extra_Terrestial_Radiation);
   t.SetBranchAddress("Global_Tilt", &Global_Tilt);
   t.SetBranchAddress("Direct_and_Circumsolar", &Direct_and_Circumsolar);
   Int_t i;
   for(i = 0; i < t.GetEntries(); i++)
    {
      t.GetEntry(i);
      t.Show(i);
      std::cout << i << ": " << Wavelength << " " << Extra_Terrestial_Radiation << " "
                << Global_Tilt << " " << Direct_and_Circumsolar << " " << std::endl;
    }
   t->ResetBranchAddresses();
   //END Optional
   */
   //
  //Global Tilt Data Extraction from tree
  t->Draw("Global_Tilt:Wavelength","","goff");
  Double_t *pkcentre = t->GetV2();
  Double_t *glob_tilt = t->GetV1();
  //
  //Loading This data to a Slitrani Spectrum
  TLitSpectrum *spam15;
  spam15 = new TLitSpectrum("Spectrum_AM1_5G","Global Tilt Spectra AM1.5");
  for(i = 0; i < t->GetEntries(); i++)
  {

      spam15->AddOneComponent(pkcentre[i],4,glob_tilt[i],-1.0,1);
  }
  spam15->Normalize();
  //
  // Resize Spectrum draw to fit whole data set, NOTE: Other graphs modified in histparams.xml
  TLitPhys::Get()->SetMinMaxWL(2.00000000000000000e+02,4.00000000000000000e+03);
  //____________________________________________________________________________
  //____________________________________________________________________________
  //
  // Generation of photons OR Draw Only
  //____________________________________________________________________________
  //____________________________________________________________________________
  //
  Bool_t  drawonly = kFALSE; //Set to true to JUST draw geometries, not to test them.
  Bool_t  motion = kTRUE;
  //
  if(drawonly == kTRUE)
  {
    //  gStyle->SetCanvasPreferGL(kTRUE);
    // top->Draw("ogl");


      phlaser-> RotateY(-45.0);

    top->Draw("");
  }
  else{
    //Load generated spectra
    plastic->FindSpectrum("Spectrum_AM1_5G");
    // Shifts sun just below horizon
    phlaser-> RotateY(7.5);
    //
    Int_t krun;
    const Int_t nrun = 27;
    const Int_t nbphot = 4000;
    Double_t angstep   = 7.5;
    TLit::Get()->BookCanvas();
    gTwoPad->SetAllGrey();
    gTwoPad->CommentTL(comTL);
    //  gStyle->SetCanvasPreferGL(kTRUE);
    // top->Draw("ogl");
    top->Draw("");
    TLitSpontan *laser;
    laser = new TLitSpontan("fibre","fibre","/TOP_1/TOT_1/FIB_1", kTRUE,kTRUE);
    laser->TrackToDraw(4);
    laser->MoveCradle(phlaser,kTRUE);
    for (krun=1;krun<=nrun;krun++) {
      laser->Gen(krun,nbphot);
      //rotation
      if (krun != nrun && motion) {
        phlaser-> RotateY(-(angstep));
        laser->MoveCradle(phlaser,kTRUE);
        }
    }
    gLitGp->SetTitle("wavelength of laser");
    gLitGp->Summary();
    gTwoPad->ChangePad();
    gLitGs->DoStat();
    TLit::Get()->CloseFiles();
  }
  //File Closing
   in.close();
  //
   f->Write();
}


