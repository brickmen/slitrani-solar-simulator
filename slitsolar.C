#include "Riostream.h"
#include <stdlib.h>
#include <iostream.h>
#include "TFile.h"
#include "TTree.h"
#include "TRandom.h"
#include "TCanvas.h"
#include "TString.h"
#include "TRegexp.h"

//L-System Tree Node Values
Int_t Run, Event;	 // Indexes Node
Int_t Leaf;		 // does branch terminate here? If so -> Leaf = 1 else Leaf = 0
Float_t x,y,z;		 // Stores Position of Node
Float_t Width; 		 // Width of branch
Float_t Length;		 // Length of branch
TVector3 vH;		 // Heading Vector
TVector3 vL;		 // Left Vector
TVector3 vU( 0, 0, 1.0); // Up Vector (constant in world)
TString fout;		 // Filename Final Tree Stored In


//L-System Tree Global Values
Float_t d1	= 94.74; 	// Divergence Angle 1	94.74
Float_t d2	= 132.63; 	// Divergence angle 2	132.63
Float_t a	= 18.95;	// Branching Angle	18.95
Float_t lr 	=1.109;		// Elongation Rate
Float_t vr	=1.732;		// Width Increase Rate

//L-System Tree Axiom
TString s = "!(1)F(200)/(45)A";

//Slitrani Setup
char *name    = "MDASim";
char *listing = "MDASim";
char *upcom   = "SLitrani Solar simulation";
char *downcom = "By Michael Abbott";

//Solar Spectrum Setup
TFile *f = new TFile("solarfiles.root","RECREATE");
TH1F *h1 = new TH1F("h1","basis",2002,280.0,4000.0);
TTree *t = new TTree("ntuple","data from csv file");



//User Interface and Main Program
void slitsolar() //Main Steering Function
{
  //Prepare SLitrani
  gROOT->ProcessLine(".x InitSLitrani.C(5,name,listing,upcom,downcom,kFALSE,kTRUE,kFALSE)");
  
  Splash();
  
  SolarInput(1);
  LSysTree(3);
  //
  // first value: 1 - draw only,  2 - sim with motion ,  3 - sim No Motion
  // second value: 0 - no tree,  1 - all tree,  2 - branches only
  SLitSimulation(1, 2);                                    
  

  
  
  /* Hidden Menu System
  Int_t exit;
  Int_t menu;
  Int_t back;
  
  while(exit != 1)
  {
    
    MainMenu();
    cin >> menu;
    
    if( menu == 0)
    {
      exit=1;
    }
    if( menu == 1) //Run All as Default
    {
      SolarInput(1);
      LSysTree(5);
      SLitSimulation();

      
      
    }
    if( menu == 2) // L-System Management
    {
      back = 0;
      while(back != 1)
      {
	LSysMenu();
	cin >> menu;
	
	if( menu == 0)
	{
	  back = 1;
	}
	if( menu == 1)
	{
	  back = 1;
	}
      }
    }
    if( menu == 3) // SMARTS Menu
    {
      back = 0;
      while(back != 1)
      {
	SMARTSMenu();
	cin >> menu;
	
	if( menu == 0)
	{
	  back = 1;
	}
	if( menu == 1) //Default
	{
	  SolarInput(1);
	}
	if( menu == 2) //Default With Print
	{
	  SolarInput(2);
	}
	if( menu == 3) //Manual Entry
	{
	  SolarInput(3);
	}
      }
     }
    if( menu == 4) // Simulation Menu
    {
      back = 0;
      while(back != 1)
      {
	SimMenu();
	cin >> menu;
	
	if( menu == 0)
	{
	  back = 1;
	}
	if( menu == 1) //Full
	{
	  SLitSimulation(1, 1);
	  RunSystem(kTRUE);
	  
	}
	if( menu == 2) //Static
	{
	  SLitSimulation(1, 1);
	  RunSystem(kFALSE);
	}
	if( menu == 3) //Draw
	{
	  SLitSimulation(1, 1);
	}
	if( menu == 4) //Draw World,  No Trees
	{
	  SLitSimulation(1, 0);
	}
      }
     }
    if( menu == 5) //Debug
    {
      SolarInput(1);
      LSysTree(5);
      SLitSimulation();
     
    }
   }
   */
  
}
void Splash() //Spash Screen Introduction, Lists commands
{
 
  cout << "***********************************************************************************"<< endl;
  
  cout << "Welcome to SLitSolar, a Solar simulator for SLitrani with L-System Tree Generation"<< endl;
  
  cout << "	Developed By Michael Abbott (m.d.abbott@warwick.ac.uk)"<< endl;
  
  cout << "			Version 0.9.1 (18 Sep 2013)"<< endl;

  cout << "***********************************************************************************"<< endl;

  cout << "Please Enter Numerical Value to Navigate Menu eg: '0' to quit"<< endl;

}
void MainMenu() //Main Menu , Lists commands
{
  cout << "-------------------------------------------------"<< endl;
  cout << "Main Menu:    	(* = Not Yet Functional)"<< endl;
  cout << "	0 - Quit"<< endl;
  cout << "	1 - Run All As Default"<< endl;
  cout << "	2 - L-System Management"<< endl;
  cout << "	3 - SMARTS Solar Input"<< endl;
  cout << "	4 - Simulation Management"<< endl;
  cout << "	5 - **Debug**"<< endl;

}
void LSysMenu() //L-System Menu, Lists commands
{
  cout << "-------------------------------------------------"<< endl;
  cout << "L-System Menu:    	(* = Not Yet Functional)"<< endl;
  cout << "	0 - Back"<< endl;
  cout << "	* - Run L-System As Default"<< endl;
  cout << "	* - Edit Axiom"<< endl;
  cout << "	* - Change Global Variables"<< endl;
  cout << "	* - Draw Tree"<< endl;

}
void SMARTSMenu() //SMARTS Menu, Lists commands
{
  cout << "-------------------------------------------------"<< endl;
  cout << "SMARTS Menu:    	(* = Not Yet Functional)"<< endl;
  cout << "	0 - Back"			<< endl;
  cout << "	1 - Use Default (ASTMG173.csv)"	<< endl;
  cout << "	2 - Use Default & Print to Screen"	<< endl;
  cout << "	3 - Manually Specify Smarts File"	<< endl;
  cout << "	* - Draw Spectra"			<< endl;

}                                                          
void SimMenu()
{
  cout << "-------------------------------------------------"<< endl;
  cout << "Simulation Menu:    	(* = Not Yet Functional)"<< endl;
  cout << "	0 - Back"			<< endl;
  cout << "	1 - Run Full Simulation"	<< endl;
  cout << "	2 - Run Static Simulation"	<< endl;
  cout << "	3 - Draw Only"	<< endl;
  cout << "	4 - Draw World Only (no tree)"			<< endl;


}
void LSysTree(Int_t n) //Manages L-System Commands, "n" is number of derivations
{
    TString foutname = "run_000";
    TString fextension = ".root";
    for (Int_t i=0; i < n; i++) {

        cout << "Run Number: "<< i << endl;
        OutputString();
      // Num is assigned for filename and then added into the string, works for up to 99 installments
        const char* num;
        num = Form( "%d", i);
        foutname.Replace(4,3,num);
        fout = foutname + fextension;
        cout  << "  File output:" << fout << endl;

        WriteTree( fout, i);

        //Runs String DerivationString
        DerivationString(i);

	


    }

}

//L-System - String Functions
void ChangeString( TString s2 ) //Applies New string
{

  s.Remove(0,s.Length());
  s.Replace(0,1,s2);


}
void DerivationString(Int_t run_number)
{

  TString sd = "";
  TString svalue = "";
  Float_t fvalue;

  
  Int_t i;

  for(i = 0; i <= s.Length(); i++)
  {

    if(s[i] == 'F')
    {
     sd += "F";
     
     //Temp Location to store string with float value
     svalue = "";
     
     
     //open value
     i++;
     sd += "(";
     
     //Read Value
     //Number to Double_t
     i++;
     while( s[i] != ')')
     {
       svalue += s[i];
       i++;
     }
     fvalue = svalue.Atof();
     //Apply Changes to value
     fvalue = (fvalue*lr);
     
     
     //Write to String
     svalue = Form( "%g", fvalue);
     sd += svalue;
     
     //Close the parenthesis
     sd += ")";
     i++;
     
    }
    if(s[i] == '!')
    {
     sd += "!";
     
     //Temp Location to store string with float value
     svalue = "";
     
     //open value
     i++;
     sd += "(";
     
     //Read Value
     //Number to Double_t
     i++;
     while( s[i] != ')')
     {
       svalue += s[i];
       i++;
     }
     fvalue = svalue.Atof();
     //Apply Changes to value
     fvalue = (fvalue*vr);
     
     
     //Write to String
     svalue = Form( "%g", fvalue);
     sd += svalue;
     
     //Close the parenthesis
     sd += ")";
     i++;
     
    }
    if(s[i] == 'A')
    {
      
      sd += "!(";
      
      fvalue = vr;
      svalue = Form( "%g", fvalue);
      sd += svalue;
      
      sd += ")F(50)[&(";
      
      fvalue = a;
      svalue = Form( "%g", fvalue);
      sd += svalue;
      
      sd += ")F(50)A]/(";
      
      fvalue = d1;
      svalue = Form( "%g", fvalue);
      sd += svalue;
      
      sd += ")[&(";
      
      fvalue = a;
      svalue = Form( "%g", fvalue);
      sd += svalue;
      
      sd += ")F(50)A]/(";
      
      fvalue = d2;
      svalue = Form( "%g", fvalue);
      sd += svalue;
      
      sd += ")[&(";
      
      fvalue = a;
      svalue = Form( "%g", fvalue);
      sd += svalue;
      
      sd += ")F(50)A]";
      
    }
    else
    {
     sd += s[i];
    }

  }
  ChangeString(sd);


}
void OutputString() //Prints current String
{

    cout << "      String set to: "<< s << endl;

}

//L-System - Tree Functions
void WriteTree(const char* filename, const Int_t run_number) //This Writes the Tree structure from the Command string
{

  //Creates The file, overwites it is it already exists
    TFile *f = new TFile(filename,"recreate");
    TTree *T = new TTree("Tnodes","Tree Structure");
    
    //Heading Vector floats for storing to tree
    Float_t vHx,vHy,vHz;

   //Links parameters to the Tree structure
   T->Branch("Run",&Run,"Run/I");
   T->Branch("Event",&Event,"Event/I");
   T->Branch("Leaf",&Leaf,"Leaf/I");
   T->Branch("x",&x,"x/F");
   T->Branch("y",&y,"y/F");
   T->Branch("z",&z,"z/F");
   T->Branch("Width",&Width,"Width/F");
   T->Branch("Length",&Length,"Length/F");
   //Only headings are stored
   T->Branch("vHx",&vHx,"vHx/F");
   T->Branch("vHy",&vHy,"vHy/F");
   T->Branch("vHz",&vHz,"vHz/F");
   
   
   //Calculates length of string
    Int_t nchar = s.Length();   // Number of Characters to read

    cout << "	Number of string characters to read, n:" << nchar <<endl;
    
    //Temporary variables for position push total needed in array is equal to run number
    Float_t px[run_number] = {};
    Float_t py[run_number]= {};
    Float_t pz[run_number]= {};
    Float_t pHx[run_number]= {};
    Float_t pHy[run_number]= {};
    Float_t pHz[run_number]= {};
    Int_t push_position = -1; //Initialises storage as first command is to increase this by 1

    cout << "	Push Mem Allocated"  <<endl;
    //Initial Set of positions, ie. ground Zero, zero Length Branch
    vH = vU; //Set heading to point up
    cout << "	Init vH"  <<endl;
    cout<<"vH = x:" << vH.X() << "  y:" << vH.Y() << "  z:" << vH.Z() << endl;
    vHx = vH.X();
    vHy = vH.Y();
    vHz = vH.Z();
    x = 0;
    y = 0;
    z = 0;
    Run = run_number;
    Event = 0 ;
    Leaf = 0 ;
    Width = 1.0 ;
    Length = 0.0 ;
    cout << "	Ready to Fill Tree"  <<endl;
    
    
    T->Fill();
    

    vL = vH.Orthogonal();
    cout << "	Init vL"  <<endl;
    
    cout << "	Init State Set"  <<endl;
    
    // Loop over n entries in string and fill the tree:
    for (Int_t i=0; i < nchar; i++) {

       //cout << "	string character i:" << i << "  s[i]:" << s[i] << endl;
       
      //Reset These
       TString svalue = "";
       Float_t fvalue;
      
      if(s[i]=='!') //Set Branch Width
      {
	
        
    
	//Temp Location to store string with float value
	svalue = "";

	//open value
	i++;

        //Read Value in Brackets
        //Number to Float_t
        i++;

        while( s[i] != ')')
        {
          svalue += s[i];

          i++;
        }
        fvalue = svalue.Atof();

     
 
	//cout << "	string value, fvalue:" << fvalue << endl;
        
        //Set Value
        Width = fvalue;

	
      } 
      if(s[i]=='F') //Straight Line, and branch growth
      {
	
        
    
	//Temp Location to store string with float value
	svalue = "";
	
	//open value
	i++;
	
        //Read Value in Brackets
        //Number to Float_t
        i++;
        while( s[i] != ')')
        {
          svalue += s[i];
          i++;
        }
        fvalue = svalue.Atof();
     
      
	//cout << "	string value, fvalue:" << fvalue << endl;
 	
	//Sets Values for Tree Storage
	Event ++;
        Leaf = 0; 
        Length = fvalue;
	vHx = vH.X();
	vHy = vH.Y();
	vHz = vH.Z();
        T->Fill();
	
	//Moves Position for Next Branch
	x = x + fvalue*( vH.X() );
        y = y + fvalue*( vH.Y() );
        z = z + fvalue*( vH.Z() );
      }
      if(s[i]=='A') //Apex, this is leaf position
      {
	
	//Sets value for Apex
         Event ++;
         Leaf = 1;
	 Length = 0;
	 Width = 0;
	 x = x;
         y = y;
         z = z; 
	 vHx = vH.X();
	 vHy = vH.Y();
	 vHz = vH.Z();
	 T->Fill();
      }
      if(s[i]=='[') //Push Position
      {

          //Push Position
          push_position ++;
	 
          px[push_position] = x;
          py[push_position] = y;
          pz[push_position] = z;
          
          pHx[push_position] = vH.X();
          pHy[push_position] = vH.Y();
	  pHz[push_position] = vH.Z();

      }
      if(s[i]==']') //Pop Position
      {
          //Pop Position

          x = px[push_position];
          y = py[push_position];
          z = pz[push_position];
          
	  vH.SetXYZ(pHx[push_position],pHy[push_position],pHz[push_position]);
	  vL = vH.Orthogonal();
       
	  
          push_position --;


      }   
      if(s[i]=='/') //Rotate about H, turn L by angle
      {

	//Temp Location to store string with float value
	svalue = "";
	
	//open value
	i++;
	
        //Read Value in Brackets
        //Number to Float_t
        i++;
        while( s[i] != ')')
        {
          svalue += s[i];
          i++;
        }
        fvalue = svalue.Atof();

	//cout << "	string value, fvalue:" << fvalue << endl;
	
	vL.Rotate(fvalue*TMath::DegToRad(), vH);

      }
      if(s[i]=='&') //Rotate about L, turn H by angle
      {

	//Temp Location to store string with float value
	svalue = "";
	
	//open value
	i++;
	
        //Read Value in Brackets
        //Number to Float_t
        i++;
        while( s[i] != ')')
        {
          svalue += s[i];
          i++;
        }
        fvalue = svalue.Atof();
     
	//cout << "	string value, fvalue:" << fvalue << endl;
	
	vH.Rotate(fvalue*TMath::DegToRad(), vL);

      }
      
     
     


    }
    cout<<"Completed Run:" << run_number <<"  Entries in Tree: "<< T->GetEntries() <<endl;

    // Write tree to file:

    T->Write("", TObject::kOverwrite);
    delete f;
    cout<<"Completed Run: Tree Saved" <<endl;


}
void PrintNodeEntry(const char* fname, Int_t line_identifier) //Prints Details for specific Node entry
{
    TFile *f = new TFile(fname);
    TTree *T = (TTree*)f->Get("Tnodes");
    T->GetEntry(line_identifier);
    T->Show(line_identifier);
}
void PrintTreeDetails(const char* fname) //Overall Details of Node Tree
{
    TFile *f = new TFile(fname);
    TTree *T = (TTree*)f->Get("Tnodes");
    T->Print();
}
void Browser() //Loads Browser
{
    new TBrowser();
}
void DrawNodes2D(const char* fname, const char* vars) //Draws Nodes in 2 Dimensions
{
    TFile *f = new TFile(fname);
    TTree *T = (TTree*)f->Get("Tnodes");

    T->Draw(vars,"","goff");
    Double_t *var2 = T->GetV2();
    Double_t *var1 = T->GetV1();
    
    //Draws The branches ending in another node
    TGraph *tr = new TGraph(T->GetEntries());
    
    for (Int_t i=0; i<T->GetEntries(); i++) {
	  tr->SetPoint(i,var1[i],var2[i]);
    }
    
    
    tr->Draw("AC*");
    

}

//SLitrani - Simulation Commands
void SLitSimulation(Int_t funct, Int_t treefunct) //Constructs the Geometry, and runs the simulation or just draws
{
  
  TGeoManager *geom = new TGeoManager("setup","Solar Panel of new Litrani");
  
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
  //
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
  //
  //  (5) Definition of wrapping medium aluminium for trunk and branches
  //
  const Double_t Air_RefrIndex     = 1.0003;   //Refractive index of air
  const Double_t aluminium_diffus  = 0.01;
  const Double_t AluminiumSupplAbs = 0.1;
  TGeoMaterial *aluminium_mat = new TGeoMaterial("Aluminium",IrrA,IrrZ,IrrRho);
  TLitMedium *aluminium = new TLitMedium("Aluminium",mediumindex++,aluminium_mat,1.0);
  aluminium->SetAsWrapping(aluminium_diffus,"RIndexRev_Aluminium","IIndexRev_Aluminium",1.0,AluminiumSupplAbs);
  aluminium->IsIsotropic(Air_RefrIndex);
  //____________________________________________________________________________
  //
  // Fixed Dimensions
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
  // Dimensions of Fibre (Sun Source)
  //
  Double_t dist          = (top_rmax - 0.51);
  Double_t delta         = (0.009308);
  Double_t fib_r         = 10*(delta * 0.5 * dist) ;
  const Double_t fib_dz  = 0.1;
  //___________________________________________________________________________
  //
  // Positionning (translations)
  //____________________________________________________________________________
  //
  // Positioning the Tree in centre of world
  //
  TGeoTranslation *tree_pstn = new TGeoTranslation("tree_pstn",0.0,0.0,0.0);
  //
  //Positioning the Light source
  //
  Double_t t_sun_orbital_raduis = top_rmax - 0.51;
  Double_t phi,theta,psi;
  TGeoRotation rsun;
  rsun.SetAngles( 90.0 , 90.0 , 0.0);
  TGeoTranslation tsun(t_sun_orbital_raduis,0.0,0.0);
  TGeoCombiTrans *csun = new TGeoCombiTrans(tsun,rsun);
  TGeoHMatrix *phsun = new TGeoHMatrix(*csun);
  //____________________________________________________________________________
  //
  // Fixed Volumes and nodes
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
  // The sun, made of plastic
  //
  TGeoVolume *fib = geom->MakeTube("FIB",plastic,0.0,fib_r,fib_dz);
  tot->AddNode(fib,1, phsun);
  //  We have to associate a TLitVolume to this TGeoVolume, since it is a source of photons
  TLitVolume *lit_fib = new TLitVolume(fib);
  TVector3 emisdir(0.0,0.0,-1.0); // axis around which photons are emitted
  TVector3 facedir(0.0,0.0,1.0); // axis to follow to reach face of emission
  TVector3 source(0.0,0.0,0.0);  // irrelevant
  lit_fib->SetEmission(flat,180.0,emisdir,"",kFALSE,source,kTRUE,facedir);
  
  // Colors and drawing
  top->SetVisibility(kFALSE);
  top->SetVisContainers();
  top->SetLineColor(1);
  top->SetLineWidth(1);
  tot->SetVisibility(kTRUE);
  fib->SetVisibility(kTRUE);
  fib->SetLineColor(FibreColor);
  fib->SetLineWidth(1);
  gGeoManager->SetTopVisible(1);
  gGeoManager->SetVisLevel(4);

  
  if( treefunct != 0 )//Draw Complete Tree 
  {
    //Load Tree Values from tree
    TFile *f = new TFile(fout);
    TTree *T = (TTree*)f->Get("Tnodes");
    Long64_t nlines = T->GetEntries();
    Long64_t file_line;
    Float_t vHx,vHy,vHz;
    //Links parameters to the Tree structure
    T->SetBranchAddress("Run",&Run);
    T->SetBranchAddress("Event",&Event);
    T->SetBranchAddress("Leaf",&Leaf);
    T->SetBranchAddress("x",&x);
    T->SetBranchAddress("y",&y);
    T->SetBranchAddress("z",&z);
    T->SetBranchAddress("Width",&Width);
    T->SetBranchAddress("Length",&Length);
    T->SetBranchAddress("vHx",&vHx);
    T->SetBranchAddress("vHy",&vHy);
    T->SetBranchAddress("vHz",&vHz);
    
    
    
    //Build Branches
    for (file_line=0; file_line < nlines; file_line++)
    {
      cout << "Line_Number : "<< file_line << endl;
      T->GetEntry(file_line);
      
      //set Heading Vector
      vH.SetXYZ(vHx,vHy,vHz);
     
      
      if( treefunct == 2 && Leaf == 0) // -> branches only
      {
	cout << "	Branch At Number" << endl;
	const char* num;
	num = Form( "%d", file_line);
	TString geooutname = "br_000";
	geooutname.Replace(3,3,num);
	cout  << "	Geometry output:" << geooutname << endl;
	
	/*
	TString geopstnname = "pstn_";
	geopstnname += geooutname;
	cout  << "	Geometry position:" << geopstnname << endl;
	TGeoTranslation *tree_position = new TGeoTranslation(geopstnname,x,y,z);
	TMath::RadToDeg()
	*/
	cout  << "	Phi output:" << TMath::RadToDeg()*vH.Phi() << endl;
	cout  << "	theta output:" << TMath::RadToDeg()*vH.Theta() << endl;
	TGeoRotation rbranch;
        rbranch.SetAngles(  TMath::RadToDeg()*vH.Phi(), TMath::RadToDeg()*vH.Theta(), 0);
        TGeoTranslation tbranch(x,y,(z+(0.5*Length)));
        TGeoCombiTrans *cbranch = new TGeoCombiTrans(tbranch,rbranch);
        TGeoHMatrix *phbranch = new TGeoHMatrix(*cbranch);
	
	TGeoVolume *branch = geom->MakeTube(geooutname,plastic,0.0,(Width),(Length));
	
	tot_disc->AddNode(branch,file_line, phbranch);
	
      }
      if( treefunct == 2 && Leaf == 1) // -> leaves only
      {
	cout << "	Leaf At Number " << endl;
	const char* num;
	num = Form( "%d", file_line);
	TString geooutname = "le_000";
	geooutname.Replace(3,3,num);
	cout  << "	Geometry output:" << geooutname << endl;
	//TGeoVolume *fib = geom->MakeTube("FIB",plastic,0.0,fib_r,fib_dz)
      }
      
      
    }
    
    
    /*

    TGeoTranslation *tree_base = new TGeoTranslation("tree_base",0.0,0.0,0.0);
    TGeoVolume *panel = geom->MakeBox("PANEL",silicon,panel_dx,panel_dy,panel_dz);
    tot->AddNode(panel,1,panel_pstn);
    TLitVolume *lit_panel = new TLitVolume(panel);
    lit_panel->SetDetector(kFALSE, "", 180.0, 270.);
    // Branches


    // Adding Leaves
    TGeoVolume *panel = geom->MakeBox("PANEL",silicon,panel_dx,panel_dy,panel_dz);



    //Adding to the world
    tot->AddNode(tree,1,tree_pstn);

    // Setting Leaves as Detectors

    for() //here needs to go a for statement covering each leaf
    {
	TLitVolume *lit_panel = new TLitVolume(panel);
	lit_panel->SetDetector(kFALSE, "", 180.0, 270.);
    }

    //____________________________________________________________________________
    //
    // Constructing the Tree
    //____________________________________________________________________________
    //
    //Basic Constuction in use
    const Double_t panel_dx = 4.0;
    const Double_t panel_dy = 4.0;
    const Double_t panel_dz = 0.1;
    Double_t t_panel_tot_z = (2*panel_dz+0.02);
    TGeoTranslation *panel_pstn = new TGeoTranslation("panel_pstn",0.0,0.0,t_panel_tot_z);
    TGeoVolume *panel = geom->MakeBox("PANEL",silicon,panel_dx,panel_dy,panel_dz);
    tot->AddNode(panel,1,panel_pstn);
    TLitVolume *lit_panel = new TLitVolume(panel);
    lit_panel->SetDetector(kFALSE, "", 180.0, 270.);
    // Branches
  
  
  
    // Adding Leaves
    TGeoVolume *panel = geom->MakeBox("PANEL",silicon,panel_dx,panel_dy,panel_dz);
  
  
  
    //Adding to the world
    tot->AddNode(tree,1,tree_pstn);
  
    // Setting Leaves as Detectors
  
    for() //here needs to go a for statement covering each leaf
    {
	TLitVolume *lit_panel = new TLitVolume(panel);
	lit_panel->SetDetector(kFALSE, "", 180.0, 270.);
    }
    */
  
  }
  

  //Close the Geometry, checking for overlaps
  geom->CloseGeometry();
  geom->CheckOverlaps(0.01);
  
  if(funct == 1) //Draw only in x3d
  {
      tot->SetVisibility(kFALSE);
      phsun-> RotateY(-45.0);

      top->Draw("x3d");
  }
  else //Run Sumulation
  {
      //Load generated spectra
      plastic->FindSpectrum("Spectrum_AM1_5G");
      // Shifts sun just below horizon
      phsun-> RotateY(7.5);
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
      TLitSpontan *solar_source;
      solar_source = new TLitSpontan("Sun","Sun","/TOP_1/TOT_1/FIB_1", kTRUE,kTRUE);
      solar_source->TrackToDraw(4);
      solar_source->MoveCradle(phsun,kTRUE);
      for (krun=1;krun<=nrun;krun++) {
	solar_source->Gen(krun,nbphot);
	//rotation
	if (krun != nrun && funct == 2 ) //Run with Motion                         
	{
	  phsun-> RotateY(-(angstep));
	  solar_source->MoveCradle(phsun,kTRUE);
	}
      }
      gLitGp->SetTitle("wavelength of solar_source");
      gLitGp->Summary();
      gTwoPad->ChangePad();
      gLitGs->DoStat();
      TLit::Get()->CloseFiles();
  }

 
}

int SolarInput(Int_t solaroptions) // Uses a SMARTS output to make a spectra for SLitrani Solar source
{

 
      
      if( solaroptions == 1)
        {
          cout << "Using Default Settings"<< endl;
          TString finname = "ASTMG173.csv"; //Name of Smarts file in Use
          Bool_t  printentries = kFALSE;
        }
      if( solaroptions == 2)
        {
          cout << "Using Default Settings with Screen Print"<< endl;
          TString finname = "ASTMG173.csv"; //Name of Smarts file in Use
          Bool_t  printentries = kTRUE;
        }
      if( solaroptions == 3)
        {
          TString finname;
          cout << "Manually enter Details: File input name (eg:ASTMG173.csv):"<< endl << flush;
          cin >> finname;
      
          Double_t yes_no;
          cout << "Do You Want a Screen Print? (yes=(1)/no=(0)):"<< endl << flush;
          cin >> yes_no;
          if( yes_no == 1)
            {
                Bool_t  printentries = kTRUE;
            }
          if( yes_no == 0)
            {
                Bool_t  printentries = kFALSE;
            }
	}
	



    
   //
   cout << "SMARTS File Input Name     : " << finname << endl;
   //
   //
   TString dir = gSystem->UnixPathName(gInterpreter->GetCurrentMacroName());
   dir.ReplaceAll("slitsolar.C","");
   dir.ReplaceAll("/./","/");
   ifstream in;
   in.open(Form( finname ,dir.Data()));
   //
   //
   //Built In Routine to read file into a tree
   t->ReadFile(finname,
             "Wavelength:Extra_Terrestial_Radiation:Global_Tilt:Direct_and_Circumsolar");

   //Prints File Contents to Screen
   if(printentries)
     {
       Double_t Wavelength, Extra_Terrestial_Radiation, Global_Tilt, Direct_and_Circumsolar;
       t->SetBranchAddress("Wavelength", &Wavelength);
       t->SetBranchAddress("Extra_Terrestial_Radiation", &Extra_Terrestial_Radiation);
       t->SetBranchAddress("Global_Tilt", &Global_Tilt);
       t->SetBranchAddress("Direct_and_Circumsolar", &Direct_and_Circumsolar);
       Int_t i;
       for(i = 0; i < t.GetEntries(); i++)
       {
         t.GetEntry(i);
         t.Show(i);
         std::cout << i << ": " << Wavelength << " " << Extra_Terrestial_Radiation << " "
                << Global_Tilt << " " << Direct_and_Circumsolar << " " << std::endl;
       }
       t->ResetBranchAddresses();
      }


    //Global Tilt Data Extraction from tree
    t->Draw("Global_Tilt:Wavelength","","goff");
    Double_t *pkcentre = t->GetV2();
    Double_t *glob_tilt = t->GetV1();
    //
    //Loading This data to a Slitrani Spectrum
    TLitSpectrum *spam15;
    spam15 = new TLitSpectrum("Spectrum_AM1_5G","Global Tilt Spectra AM1.5");
    Int_t i;
    for(i = 0; i < t->GetEntries(); i++)
      {
          spam15->AddOneComponent(pkcentre[i],4,glob_tilt[i],-1.0,1);
      }
    spam15->Normalize();
    //
    // Resize Spectrum draw to fit whole data set, NOTE: Other graphs modified in histparams.xml
    TLitPhys::Get()->SetMinMaxWL(2.00000000000000000e+02,4.00000000000000000e+03);

      //File Closing
   in.close();
  //
   f->Write();
    return 1;
}
