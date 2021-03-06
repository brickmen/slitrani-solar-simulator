// Unsure if all of these are needed but they are all here
#include "Riostream.h"
#include <stdlib.h>
#include <iostream.h>
#include "TFile.h"
#include "TTree.h"
#include "TRandom.h"
#include "TCanvas.h"
#include "TString.h"
#include "TRegexp.h"

//L-System Tree Node Values. These are accessed and used by most functions hence its easier to have these as global defined
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
//Edit these to change Tree Growth directions, defaluts are in comments
Float_t d1	= 120; 	// Divergence Angle 1	Default:94.74
Float_t d2	= 120; 	// Divergence angle 2	Default:132.63
Float_t a	= 25;	// Branching Angle	Default:18.95
Float_t lr 	= 1.109;	// Elongation Rate	Default:1.109
Float_t vr	= 1.732;	// Width Increase Rate	Default:1.732

//L-System Tree Axiom
TString s = "!(1)F(50)/(45)A"; //This is the initial tree start point, Default: "!(1)F(200)/(45)A"
Int_t derivations = 7; //Number of derivations, more = larger tree = longer process time

//Slitrani comments and filenames
// Editing these changes the filename SLitrani Uses and the name that appears on the splashscreen
char *name    = "SLitSim";
char *listing = "SLitSim";
char *upcom   = "SLitrani Solar simulation";
char *downcom = "By Michael Abbott";

//Tree Scale, the tree constructed is too large to fit in SLitrani's world so a scale factor is introduced
Float_t Scale = 0.009; //Constuction scale down of L-System to fit in world


//User Interface and Main Program
void slitsolar()
{
  //Prepare SLitrani - Must be run before SLitSimulation AND SolarInput, easiest to run this first
  gROOT->ProcessLine(".x InitSLitrani.C(5,name,listing,upcom,downcom,kFALSE,kTRUE,kFALSE)");
  //The SpashScreen
  Splash(); 
}
void Auto() //Main Steering Function this can be used to Automatically run the system
{
  //Loads SMARTS File
  SolarInput(1);
  //options:
  //	1-Uses Default SMARTS file defined in this code as: ASTMG173.csv
  //	2-Same as above but includes a print to screen of each entry
  //	3-Manual entry, gives command line control
  
  //Prepares L-System String and Calculats position and angles of nodes
  LSysTree(derivations);
  // Option: Number of derivations for Tree Construction
  // 	!!Caution: Greater this number, the longer it takes to run the simulation
  
  //Runs the Simulation in SLitrani
  SLitSimulation(2, 1,fout);                                    
  // first option, Simulation:
  //		1 - draw only in oGL
  //		2 - Run Simulation with sun motion 
  //		3 - sim No Motion
  // second option, Tree Construction: 
  //		0 - no tree,  
  //		1 - all tree,  
  //		2 - branches only
  //Third Option: Name of tree( run_**.root) file in use for constuction
  //		Default: fout (the final .root file made in WriteTree)

}
void Splash() //Spash Screen Introduction
{
 
  cout << "***********************************************************************************"<< endl;
  
  cout << "Welcome to SLitSolar, a Solar simulator for SLitrani with L-System Tree Generation"<< endl;
  
  cout << "	Developed By Michael Abbott (m.d.abbott@warwick.ac.uk)"<< endl;
  
  cout << "			Version 1.0.0 (23 Sep 2013)"<< endl;

  cout << "***********************************************************************************"<< endl;

  cout << "Main program can be run by typing: > 'Auto()'; type > 'Help()' for list of commands."<< endl;

}
void Help() //Lists commands
{
  cout << "Congratulations for sucessfully installing SLitrani!"<< endl;
  cout << endl;
  cout << "There are 3 sections to this applicaton, each is reliant on the previous being run."<< endl;
  cout << endl;
  cout << "Stage 1, Solar Input:-> SolarInput(num)"<< endl;
  cout << "	What it does: Opens the SMARTS file for SLitrani to make a solar realistic spectrum output"<< endl;
  cout << "	Options Available: num"<< endl;
  cout << "		num = 1 -> Uses Default SMARTS file defined in this code as: ASTMG173.csv"<< endl;
  cout << "		num = 2 -> Same as above but includes a print to screen of each entry"<< endl;
  cout << "		num = 3 -> Manual entry, gives command line control"<< endl;
  cout << "	Example Command Input: >SolarInput(1)"<< endl;
  cout << endl;
  cout << "Stage 2, L-System Generation:-> LSysTree(num)"<< endl;
  cout << "	What it does: Uses a built in Axiom and derivation based "<< endl;
  cout << "			on an L-System from Figure 2.8 of The Algorithmic Beauty of Plants."<< endl;
  cout << "	Options Available: num"<< endl;
  cout << "		num = Positive Integer -> Number of derivation steps in L-System"<< endl;
  cout << "		!NOTE: Higher values exponentially increase process time, values ~7 are reasonable."<< endl;
  cout << "	Example Command Input: >LSysTree(5)"<< endl;
  cout << endl;
  cout << "Stage 2, L-System Generation:-> SLitSimulation(num1, num2, fout)"<< endl;
  cout << "	What it does: Constructs the SLitrani world and the L-System, can also run the simulation."<< endl;
  cout << "	Options Available: num1, num2, fout"<< endl;
  cout << "		num1 = 1 -> Draw the system only"<< endl;
  cout << "		num1 = 2 -> Run the Simulation with solar motion"<< endl;
  cout << "		num1 = 3 -> Run the simulation with static sun"<< endl;
  cout << "		num2 = 0 -> World With no tree constucted"<< endl;
  cout << "		num2 = 1 -> Builds All the Tree"<< endl;
  cout << "		fout = '''' -> Chooses the run_XX.root file to use;"<< endl;
  cout << "			'fout' with no quotations automatically uses the final .root file"<< endl;
  cout << "	Example Command Input: >SLitSimulation(2, 1, fout) or >SLitSimulation(2, 1, ''run_3.root'') "<< endl;
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

    //cout << "s [i]"<< s[i] << endl;
    
    if(s[i] == 'F')
    {
     sd += "F";
     //cout << "reading an F"<< endl;
     //Temp Location to store string with float value
     svalue = "";
     
     
     //open value, char is now (
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
     
     
    }
    else if(s[i] == '!')
    {
     sd += "!";
     
     //Temp Location to store string with float value
     svalue = "";
     
     //open value, char is now (
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

    
     
    }
    else if(s[i] == 'A')
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
    
    //Vector floats for storing to tree
    Float_t vHx,vHy,vHz;
    Float_t vLx,vLy,vLz;

   //Links parameters to the Tree structure
   T->Branch("Run",&Run,"Run/I");
   T->Branch("Event",&Event,"Event/I");
   T->Branch("Leaf",&Leaf,"Leaf/I");
   T->Branch("x",&x,"x/F");
   T->Branch("y",&y,"y/F");
   T->Branch("z",&z,"z/F");
   T->Branch("Width",&Width,"Width/F");
   T->Branch("Length",&Length,"Length/F");
   T->Branch("vHx",&vHx,"vHx/F");
   T->Branch("vHy",&vHy,"vHy/F");
   T->Branch("vHz",&vHz,"vHz/F");
   T->Branch("vLx",&vLx,"vLx/F");
   T->Branch("vLy",&vLy,"vLy/F");
   T->Branch("vLz",&vLz,"vLz/F");
   
   
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
    Float_t pLx[run_number]= {};
    Float_t pLy[run_number]= {};
    Float_t pLz[run_number]= {};
    Float_t pLz[run_number]= {};
    Float_t pWidth[run_number]= {};
    Int_t push_position = -1; //Initialises storage as first command is to increase this by 1

    //cout << "	Push Mem Allocated"  <<endl;
    //Initial Set of positions, ie. ground Zero, zero Length Branch
    vH = vU; //Set heading to point up
    //cout << "	Init vH"  <<endl;
    //cout<<"vH = x:" << vH.X() << "  y:" << vH.Y() << "  z:" << vH.Z() << endl;
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
    
    
    
    T->Fill();
    

    //initial setup for vL
    vL = vH.Orthogonal();
    //cout << "	Init vL"  <<endl;
    
    //cout << "	Init State Set"  <<endl;
    cout << "	Initial State Set: Ready to Fill Tree"  <<endl;
    
    // Loop over n entries in string and fill the tree:
    for (Int_t i=0; i < nchar; i++) {

       //cout << "	string character i:" << i << "  s[i]:" << s[i] << endl;
       
      //Reset These
       TString svalue = "";
       Float_t fvalue;
      
      if(s[i]=='!') //Set Branch Width
      {

	//open value, char is now (
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

     
 
	//comments
	cout << "	! Width |Before:" << Width << endl;
        
        //Set Value
        Width = fvalue;
	
	//comments
	cout << "	! Width |After:" << Width << endl;

	
      } 
      else if(s[i]=='F') //Straight Line, and branch growth
      {

	//open value, char is now (
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
 	
	//Stores Node
	//Sets Values for Tree Storage
	Event ++;
        Leaf = 0; 
        Length = fvalue;
	vHx = vH.X();
	vHy = vH.Y();
	vHz = vH.Z();
	vLx = vL.X();
	vLy = vL.Y();
	vLz = vL.Z();
	cout << "	Width |Set:" << Width << endl;
        T->Fill();
	
	cout<<"		F Position |Before = x:" << x << "  y:" << y << "  z:" << z << endl;
	
	//Moves Position for Next Branch
	x = x + fvalue*( vH.X() );
        y = y + fvalue*( vH.Y() );
        z = z + fvalue*( vH.Z() );
	
	cout<<"		F Position |After = x:" << x << "  y:" << y << "  z:" << z << endl;
      }
      else if(s[i]=='A') //Apex, this is leaf position
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
	 vLx = vL.X();
	 vLy = vL.Y();
	 vLz = vL.Z();
	 T->Fill();
      }
      else if(s[i]=='[') //Push Position
      {

          //Push Position
          push_position ++;
	 
          px[push_position] = x;
          py[push_position] = y;
          pz[push_position] = z;
          
          pHx[push_position] = vH.X();
          pHy[push_position] = vH.Y();
	  pHz[push_position] = vH.Z();
	  
	  pLx[push_position] = vL.X();
          pLy[push_position] = vL.Y();
	  pLz[push_position] = vL.Z();
	  
	  pWidth[push_position] = Width;
	  
	  cout<<"	[|Push	Width: " << Width<< endl;
	  cout<<"		[Push Position |Set = x:" << x << "  y:" << y << "  z:" << z << endl;
	  cout<<"		[Push Position | vL set = x:" << vL.X() << "  y:" << vL.Y() << "  z:" << vL.Z() << endl;
	  cout<<"		[Push Position | vH set = x:" << vH.X() << "  y:" << vH.Y() << "  z:" << vH.Z() << endl;

      }
      else if(s[i]==']') //Pop Position
      {
          //Pop Position

          x = px[push_position];
          y = py[push_position];
          z = pz[push_position];
	  
	  Width = pWidth[push_position];
          
	  vH.SetXYZ(pHx[push_position],pHy[push_position],pHz[push_position]);
	  vL.SetXYZ(pLx[push_position],pLy[push_position],pLz[push_position]);
       
	  
          push_position --;

	  cout<<"	]|Pop	Width: " << Width<< endl;
	  cout<<"		]Pop Position |Set = x:" << x << "  y:" << y << "  z:" << z << endl;
	  cout<<"		]Pop Position | vL set = x:" << vL.X() << "  y:" << vL.Y() << "  z:" << vL.Z() << endl;
	  cout<<"		]Pop Position | vH set = x:" << vH.X() << "  y:" << vH.Y() << "  z:" << vH.Z() << endl;

      }   
      else if(s[i]=='/') //Rotate about H, turn L by angle
      {

	//open value, char is now (
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

	cout << "		/ Rot about H|angle:" << fvalue << endl;
	cout<<"		/ Rot about H| vH set = x:" << vH.X() << "  y:" << vH.Y() << "  z:" << vH.Z() << endl;
	cout<<"		/ Rot about H| vL Before = x:" << vL.X() << "  y:" << vL.Y() << "  z:" << vL.Z() << endl;
	
	vL.Rotate(fvalue*TMath::DegToRad(), vH);
	
	cout<<"		/ Rot about H| vL After = x:" << vL.X() << "  y:" << vL.Y() << "  z:" << vL.Z() << endl;

      }
      else if(s[i]=='&') //Rotate about L, turn H by angle
      {

	//open value, char is now (
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
     
	cout << "		& Rot about L|angle:" << fvalue << endl;
	cout<<"		& Rot about L| vL set = x:" << vL.X() << "  y:" << vL.Y() << "  z:" << vL.Z() << endl;
	cout<<"		& Rot about L| vH Before = x:" << vH.X() << "  y:" << vH.Y() << "  z:" << vH.Z() << endl;
	
	vH.Rotate(fvalue*TMath::DegToRad(), vL);
	
	cout<<"		& Rot about L| vH After = x:" << vH.X() << "  y:" << vH.Y() << "  z:" << vH.Z() << endl;

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
void PrintTreeDetails(const char* fname) //Summary of Node Tree saved in file
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
void SLitSimulation(Int_t funct, Int_t treefunct, TString filename) //Constructs the Geometry, and runs the simulation or just draws
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
  tot->AddNodeOverlap(tot_disc,1,new TGeoTranslation("totpstn",0.0,0.0,-0.0001));
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
  tot->SetVisibility(kFALSE);
  
  top->SetVisContainers();
  top->SetLineColor(1);
  top->SetLineWidth(1);
  
  fib->SetVisibility(kTRUE);
  fib->SetLineColor(FibreColor);
  fib->SetLineWidth(1);
  gGeoManager->SetTopVisible(1);
  gGeoManager->SetVisLevel(4);

  
  if( treefunct != 0 )//Draw Complete Tree 
  {
    //Load Tree Values from tree
    TFile *f = new TFile(filename);
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
    
    
    
    TGeoVolume *leaf = geom->MakeTube("LEAF",silicon,0.0, 0.2 ,0.001);
    //Set Leaves as Detectors
    
    TLitVolume *lit_leaf = new TLitVolume(leaf);
    lit_leaf->SetDetector(kFALSE, "", 180.0, 270.);
    cout << "leaf set as detector"<< endl;
    
    
    //Construct Trees
    for (file_line=0; file_line < nlines; file_line++)
    {
      cout << "Line_Number : "<< file_line << endl;
      T->GetEntry(file_line);
      
      //set Heading Vector
      vH.SetXYZ(vHx,vHy,vHz);
     
      
      if( Leaf == 0 && Length != 0) // branches
      {
	
	//rotation from heading phi, theta, psi
	TGeoRotation *rbranch = new TGeoRotation("rbranch", (TMath::RadToDeg()*vH.Phi())+90 , TMath::RadToDeg()*vH.Theta()  , 0);
	// combine with x,y,z position
	TGeoCombiTrans *combibranch = new TGeoCombiTrans( Scale*x, Scale*y, Scale*z, rbranch);
	
	//position branchtube above positioning disc
        TGeoTranslation *tbranch = new TGeoTranslation("tbranch",0., 0.,(0.5*Scale*Length));
	
	TGeoVolume *branchdisc = geom->MakeTube("branchdisc",vacuum,0.0,(Scale*Width*2),0.01);
	
	TGeoVolume *branchtub = geom->MakeTube("branchtub",plastic,0.0,(Scale*Width),(Scale*(0.5*Length)));
	
	//Add tube to disc
	branchdisc->AddNodeOverlap(branchtub,1, tbranch);
	
	//hide disc
	branchdisc->SetVisibility(kFALSE);
	

	tot->AddNodeOverlap(branchdisc,file_line, combibranch);
	
	branchtub->SetLineColor(28);
	
      }
      if( Leaf == 1) // -> leaves
      {
	/*
	 * //TString leavesscript = "";
	//cout << "	Leaf At Number " << endl;
	const char* num;
	num = Form( "%d", file_line);
	TString geooutname = "le_000";
	geooutname.Replace(3,3,num);
	TString pstnoutname = "ps_000";
	pstnoutname.Replace(3,3,num);
	cout  << "	Geometry output:" << geooutname << endl;
	cout  << "	Position output:" << pstnoutname << endl;
	
	//TGeoTube *leaf = new TGeoTube(geooutname,0.0, 0.1 ,0.001);
	//leaf->SetName(pstnoutname);
	//leaf->RegisterYourself(); 
	
	//leavesscript += geooutname+":"+pstnoutname+"+";
	
	//TLitVolume *lit_leaf = new TLitVolume(leaf);
        //lit_leaf->SetDetector(kFALSE, "", 180.0, 270.);
	   // TGeoCompositeShape *leaves = new TGeoCompositeShape("cs", leavesscript);
    */
	
	//rotation from heading phi, theta, psi
	TGeoRotation *rbranch = new TGeoRotation("rbranch", (TMath::RadToDeg()*vH.Phi())+90 , TMath::RadToDeg()*vH.Theta()  , 0);
	// combine with x,y,z position
	TGeoCombiTrans *combibranch = new TGeoCombiTrans( Scale*x, Scale*y, (Scale*z+0.001), rbranch);
	
	tot->AddNodeOverlap(leaf,file_line, combibranch);
	
      }
      
      
    }
    leaf->SetLineColor(8);
    
      


   
  }
  

  //Close the Geometry, checking for overlaps
  geom->CloseGeometry();
  geom->CheckOverlaps(0.01);
  
  if(funct == 1) //Draw only in x3d
  {
      tot->SetVisibility(kFALSE);
      tot->SetVisContainers();
      phsun-> RotateY(-45.0);

      top->Draw("ogl");
  }
  else //Run Sumulation
  {

      
      //Load generated spectra
      plastic->FindSpectrum("Spectrum_AM1_5G");
      // Shifts sun just below horizon
      phsun-> RotateY(7.5);
      //
      Int_t krun;
      const Int_t nrun = 27; //Default:27
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
    //Solar Spectrum File Setup
    TFile *f = new TFile("solarfiles.root","RECREATE");
    TH1F *h1 = new TH1F("h1","basis",2002,280.0,4000.0);
    TTree *t = new TTree("ntuple","data from csv file");

 
      
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
