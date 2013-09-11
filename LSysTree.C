#include <stdlib.h>
#include <iostream.h>
#include "TFile.h"
#include "TTree.h"
#include "TRandom.h"
#include "TCanvas.h"
#include "TString.h"
#include "TRegexp.h"


Int_t Run, Event, Leaf;
Float_t x,y,z;
Float_t theta, phi;

Float_t step = 1.0;
Float_t branchangle = 0.785398163; //45 degrees in rads

TString s = "0";


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

    if(s[i] == '1')
    {
     sd += "11";
    }
    if(s[i] == '0')
    {
     sd += "1[0]0";
    }
    if(s[i] == '[')
    {
     sd += "[";
    }
    if(s[i] == ']')
    {
     sd += "]";
    }

  }
  ChangeString(sd);


}
void LSysTree()
{
    Introduce();
    Int_t n=6; //Number of derivations
    
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

        WriteTree( fout, i);

        //Runs String DerivationString
        DerivationString(i);

	


    }

}

void WriteTree(const char* filename, const Int_t run_number)
{

  //Loads the string and fills in the tree using built in rules
    TFile *f = new TFile(filename,"recreate");
    TTree *T = new TTree("Tnodes","Tree Structure");

   T->Branch("Run",&Run,"Run/I");
   T->Branch("Event",&Event,"Event/I");
   T->Branch("x",&x,"x/F");
   T->Branch("y",&y,"y/F");
   T->Branch("z",&z,"z/F");
   T->Branch("theta",&theta,"theta/F");
   T->Branch("phi",&phi,"phi/F");
   T->Branch("Leaf",&Leaf,"Leaf/I");
   
    Int_t n=s.Length();   // Number of Characters to read

    //Temporary variables for position push MAX needed in array is equal to run number
    Float_t px[run_number] = {};
    Float_t py[run_number]= {};
    Float_t pz[run_number]= {};
    Float_t ptheta[run_number]= {};
    Float_t pphi[run_number]= {};
    Int_t push_position = -1;

    //Initial Set of positions
    x = 0;
    y = 0;
    z = 0;
    theta = 0;
    phi = 0;
    Event = 0 ;
    Leaf = 0 ;
    T->Fill();

    // Loop over n entries and fill the tree:
    // This is the Axiom
    for (Int_t i=0; i < n; i++) {
      Run = run_number;

      

      if(s[i]=='1')
      {
         Event ++;
         Leaf = 0;
         
         x = x + step*( sin(theta)*cos(phi) );
         y = y + step*( sin(theta)*sin(phi) );
         z = z + step*( cos(theta) );
	 T->Fill();
      }
      if(s[i]=='0')
      {
         Event ++;
         Leaf = 1;
         
         x = x + step*( sin(theta)*cos(phi));
         y = y + step*( sin(theta)*sin(phi));
         z = z + step*( cos(theta) );
	 T->Fill();
      }

      if(s[i]=='[')
      {

          //Push Position
          push_position ++;
	 
          px[push_position] = x;
	
          py[push_position] = y;
          pz[push_position] = z;
          ptheta[push_position] = theta;
          pphi[push_position] = phi;

          //Turn Left 45 degrees (0.785398163 radians)
          theta += -branchangle;
      }
      if(s[i]==']')
      {
          //Pop Position

          x = px[push_position];
	 
          y = py[push_position];
          z = pz[push_position];
          theta = ptheta[push_position];
          phi = pphi[push_position];
          push_position --;


          //Turn Right 45 degrees (0.785398163 radians)
          theta += branchangle;
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
    
    
    tr->Draw("A*");
    

}


void WorldDraw()
{
  TGeoManager *geom = new TGeoManager("setup","L-System Tree");
}

