#include <stdlib.h>
#include <iostream.h>
#include "TFile.h"
#include "TTree.h"
#include "TRandom.h"
#include "TCanvas.h"
#include "TString.h"
#include "TRegexp.h"

//Node Values
Int_t Run, Event;	 // Indexes Node
Int_t Leaf;		 // does branch terminate here? If so -> Leaf = 1 else Leaf = 0
Float_t x,y,z;		 // Stores Position of Node
TVector3 vH;		 // Heading Vector
TVector3 vL;		 // Left Vector
TVector3 vU( 0, 0, 1.0); // Up Vector (constant in world)
Float_t width; 		 // Width of branch

//Global Values
Float_t d1	= 94.74; 	// Divergence Angle 1
Float_t d2	= 132.63; 	// Divergence angle 2
Float_t a	= 18.95;	// Branching Angle
Float_t lr 	=1.109;		// Elongation Rate
Float_t vr	=1.732;		// Width Increase Rate

TString s = "!(1)F(200)/(45)A";


void Introduce()
{
  cout << "Welcome to LSysTree an L-System Generator for a Vector Node Tree"<< endl;
  
  cout << "Currently axiom set to: "<< s << endl;

  cout << "ChangeString() allows you to change the initial character set"<< endl;

  cout << "OutputString() Prints current stage of string"<< endl;

}
void ChangeString( TString s2 )
{

  s.Remove(0,s.Length());
  s.Replace(0,1,s2);


}
void OutputString()
{

    cout << "      String set to: "<< s << endl;

}
void DerivationString(Int_t run_number)
{

  TString sd = "";

  Int_t i;

  for(i = 0; i <= s.Length(); i++)
  {

    if(s[i] == 'F')
    {
     sd += "F";
     
     //Temp Location to store string with float value
     TString svalue = "";
     Float_t fvalue;
     
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
     TString svalue = "";
     Float_t fvalue;
     
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
      Float_t fvalue;
      
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
void LSysTree()
{
    Introduce();
    Int_t n=3; //Number of derivations
    
    TString foutname = "run_000";
    TString fextension = ".root";
    for (Int_t i=0; i < n; i++) {

        cout << "Run Number: "<< i << endl;
        OutputString();
      // Num is assigned for filename and then added into the string, works for up to 99 installments
        const char* num;
        num = Form( "%d", i);
        foutname.Replace(4,3,num);
        TString fout = foutname + fextension;
        cout  << "  File output:" << fout << endl;

       // WriteTree( fout, i);

        //Runs String DerivationString
        DerivationString(i);

	


    }

}

void WriteTree(const char* filename, const Int_t run_number) //This Writes the Tree structure from the Command string
{

  //Creates The file, overwites it is it already exists
    TFile *f = new TFile(filename,"recreate");
    TTree *T = new TTree("Tnodes","Tree Structure");

   //Links parameters to the Tree structure
   T->Branch("Run",&Run,"Run/I");
   T->Branch("Event",&Event,"Event/I");
   T->Branch("Leaf",&Leaf,"Leaf/I");
   T->Branch("x",&x,"x/F");
   T->Branch("y",&y,"y/F");
   T->Branch("z",&z,"z/F");
   //Only headings are stored
   T->Branch("vHx",&vH.X(),"vHx/F");
   T->Branch("vHy",&vH.Y(),"vHy/F");
   T->Branch("vHz",&vH.Z(),"vHz/F");
   
   //Calculates length of string
    Int_t n=s.Length();   // Number of Characters to read

    //Temporary variables for position push total needed in array is equal to run number
    Float_t px[run_number] = {};
    Float_t py[run_number]= {};
    Float_t pz[run_number]= {};
    Float_t pHx[run_number]= {};
    Float_t pHy[run_number]= {};
    Float_t pHz[run_number]= {};
    Int_t push_position = -1; //Initialises storage as first command is to increase this by 1

    //Initial Set of positions, ie. ground Zero
    vH = vU; //Set heading to point up
    x = 0;
    y = 0;
    z = 0;
    Run = run_number;
    Event = 0 ;
    Leaf = 0 ;
    T->Fill();

    // Loop over n entries in string and fill the tree:
    for (Int_t i=0; i < n; i++) {
      //Set Left Value
       vL = vH.Orthogonal();
       cout<<"vH = x:" << vH.X() << "  y:" << vH.Y() << "  z:" << vH.Z() << endl;
       cout<<"vL = x:" << vL.X() << "  y:" << vL.Y() << "  z:" << vL.Z() << endl;
       cout<<"vU = x:" << vU.X() << "  y:" << vU.Y() << "  z:" << vU.Z() << endl;
       
      
      if(s[i]=='F')
      {
	//Straight Line, and branch growth
        
    
	//Temp Location to store string with float value
	TString svalue = "";
	Float_t fvalue;
	
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
     
        //Close the parenthesis
        i++; 
 	
	Event ++;
        Leaf = 0;
         
        x = x + step*( vH.X() );
        y = y + step*( vH.Y() );
        z = z + step*( vH.Z() );
        T->Fill();
      }
      if(s[i]=='X')
      {
	//Straight Line, and branch growth, ending in Leaf
         Event ++;
         Leaf = 1;
	 
	 x = x + step*( vH.X() );
         y = y + step*( vH.Y() );
         z = z + step*( vH.Z() );
	 T->Fill();
      }
      if(s[i]=='[')
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
      if(s[i]==']')
      {
          //Pop Position

          x = px[push_position];
          y = py[push_position];
          z = pz[push_position];
          
	  v1.SetXYZ(pHx[push_position],pHy[push_position],pHz[push_position]);
	  
          push_position --;


      }   
      if(s[i]=='+')
      {
          //Rotate about U by a2 angle
	
	vH.Rotate((TMath::Pi)*(a2/180), vU);


      }
      if(s[i]=='-')
      {
          //Rotate about U by negative a2 angle
	
	vH.Rotate((TMath::Pi)*(-a2/180), vU);


      }
      if(s[i]=='&')
      {
          //Rotate about L by divergence angle
	
	vH.Rotate((TMath::Pi)*(d/180), vU);


      }
     


    }
    cout<<"Completed Run:" << run_number <<"  Entries in Tree: "<< T->GetEntries() <<endl;

    // Write tree to file:

    T->Write("", TObject::kOverwrite);
    delete f;


}



// Printing and Browsing Commands
void PrintNodeEntry(const char* fname, Int_t line_identifier)
{
    TFile *f = new TFile(fname);
    TTree *T = (TTree*)f->Get("Tnodes");
    T->GetEntry(line_identifier);
    T->Show(line_identifier);
}
void PrintTree(const char* fname)
{
    TFile *f = new TFile(fname);
    TTree *T = (TTree*)f->Get("Tnodes");
    T->Print();
}
void Browser()
{
    new TBrowser();
}

//Drawing Commands
void DrawNodes2D(const char* fname, const char* vars)
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


void WorldDraw()
{
  TGeoManager *geom = new TGeoManager("setup","L-System Tree");
}

