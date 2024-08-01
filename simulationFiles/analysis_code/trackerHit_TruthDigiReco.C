#include <iostream>
#include <vector>
#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include <TH1F.h>
#include "Math/Vector4D.h"
#include <TAttFill.h>
#include <TROOT.h>
#include <TRandom.h>
#include <TCanvas.h>
#include <sys/stat.h> // For mkdir() function
#include <string>   // Include the <string> header for std::string
#include <cstdio>   // Include the <cstdio> header for Form

using namespace std;

// --- Histogram declaration
//DIGI
TH1F* h_digi_trhit_tim;
TH1F* h_digi_trhit_edp;
//Truth
TH1F* h_truth_trhit_tim;
TH1F* h_truth_trhit_edp;
//Reco
TH1F* h_reco_trhit_tim;
TH1F* h_reco_trhit_edp;

//diff
TH1F* h_diff_edp;
TH1F* h_diff_tim;

void plot_three_overlapped_hists(TH1F *hist1, TH1F *hist2, TH1F *hist3, string dirname_, string layer_, string plotTitle, string plotXtitle, string plotYtitle, string legendName1, string legendName2, string legendName3, string type){
    // Enable OpenGL support in ROOT
    //gROOT->SetGraphicsSystem("OpenGL");
    TCanvas *canvas = new TCanvas("canvas", "canvasTitle", 800, 600);
    canvas->SetSupportGL(true);
     // Set line and fill colors for histograms
    hist1->SetLineColor(kBlue);
    //hist1->SetFillColorAlpha(kBlue, 0.3); // Set fill color with transparency (0.3)
    hist2->SetLineColor(kRed);
    //hist2->SetFillColorAlpha(kRed, 0.2); 
    //hist2->SetFillStyle(3244);
    hist3->SetLineColor(kGreen + 2);
    //hist3->SetLineWidth(3);
    // Draw histograms on the same pad
    hist1->Draw("hist");
    hist2->Draw("hist same");
    hist3->Draw("hist same");

    // Set x-axis limits
    // double xmin = -0.2;
    // double xmax = 11;
    // hist1->GetXaxis()->SetRangeUser(xmin, xmax);
    // hist2->GetXaxis()->SetRangeUser(xmin, xmax);

    //TLegend:
    TLegend *legend = new TLegend(0.65, 0.7, 0.85, 0.85); // x1, y1, x2, y2
    legend->AddEntry(hist1, Form("%s", legendName1.c_str()), "l"); // "f" for filled area
    legend->AddEntry(hist2, Form("%s", legendName2.c_str()), "l"); // "f" for filled area
    legend->AddEntry(hist3, Form("%s", legendName3.c_str()), "l");
    legend->SetBorderSize(0); // No border around legend
    legend->SetTextSize(0.03);
    legend->Draw();
    //Title:
    hist1->SetTitle(Form("%s%s", plotTitle.c_str(), layer_.c_str()));
    hist1->GetXaxis()->SetTitle(Form("%s", plotXtitle.c_str()));
    hist1->GetYaxis()->SetTitle(Form("%s", plotYtitle.c_str()));
    //hist1->SetTitleSize(0.05);

    hist1->SetStats(kFALSE); // Disable statistics box for hist1
    hist2->SetStats(kFALSE); // Disable statistics box for hist2
    hist3->SetStats(kFALSE);

   // To prevent the canvas from popping up automatically, use batch mode
    gROOT->SetBatch(kTRUE); // Enable batch mode

    canvas->SaveAs(Form("%s/%s_%s.png", dirname_.c_str(), type.c_str(),layer_.c_str()));
    canvas->Close();
    delete canvas;
}

void plot_hist(TH1F *hist, string plotTitle, string plotXtitle, string dirname_,string name){
    // Create a canvas
    TCanvas *canvas = new TCanvas("canvas", "Histogram Canvas", 800, 600);
    // Draw the histogram on the canvas
    hist->Draw();
    hist->SetTitle(Form("%s", plotTitle.c_str()));
    hist->GetXaxis()->SetTitle(Form("%s", plotXtitle.c_str()));
    hist->GetYaxis()->SetTitle("Events");

    // To prevent the canvas from popping up automatically, use batch mode
    gROOT->SetBatch(kTRUE); // Enable batch mode

    canvas->SaveAs(Form("%s/%s.png", dirname_.c_str(), name.c_str()));
    canvas->Close();
    delete canvas;
    
}

void subtract_hists(TH1F *hist1, TH1F *hist2, string plotTitle, string plotXtitle, string dirname_,string name){
    //new hist
    TH1F *hist_diff = new TH1F("hist_diff", "Histogram Difference", 50, 0, 5);
    // Subtract histograms and fill the difference histogram
    for (int bin = 1; bin <= hist1->GetNbinsX(); ++bin) {
        double bin_content_diff = abs(hist1->GetBinContent(bin) - hist2->GetBinContent(bin));
        hist_diff->SetBinContent(bin, bin_content_diff);
    }
    
    // Create a canvas
    TCanvas *canvas = new TCanvas("canvas", "Histogram Canvas", 800, 600);
    
    // Draw the difference histogram
    hist_diff->Draw();
    hist_diff->SetTitle(Form("%s", plotTitle.c_str()));
    hist_diff->GetXaxis()->SetTitle(Form("%s", plotXtitle.c_str()));
    hist_diff->GetYaxis()->SetTitle("Events");
    
    // Keep the canvas open
    canvas->Update();
    canvas->Draw();

    // To prevent the canvas from popping up automatically, use batch mode
    gROOT->SetBatch(kTRUE); // Enable batch mode

    canvas->SaveAs(Form("%s/%s.png", dirname_.c_str(), name.c_str()));
    canvas->Close();
    delete canvas;
}


void trackerHit_TruthDigiReco(const char *inputFile_truthReco, const char *inputFile_digi, std::string layer, std::string keyWord, const long int maxEvents=-1){

    TFile *file_truthReco = new TFile(inputFile_truthReco);
    TTree *myLCTuple_truthReco = (TTree*) file_truthReco->Get("MyLCTuple");
    cout << "Root file 1 is: " << inputFile_truthReco << endl;

    TFile *file_digi = new TFile(inputFile_digi);
    TTree *myLCTuple_digi = (TTree*) file_digi->Get("MyLCTuple");
    cout << "Root file 2 is: " << inputFile_digi << endl;

    // Create directory
    TString full_path = Form("tdr_plots/%s_plots", layer.c_str());
    // Convert TString to const char* for mkdir
    const char* dirname = full_path.Data();
    //string dirname = Form("%s_plots", layer.c_str());
    int dir_status = mkdir(dirname, 0777);

    int bins = 50;
    float minTim = -0.1; //ns
    float maxTim = 6; //ns
    float minEn = 0; //GeV
    float maxEn = 0.12e-3; //GeV

    //--Difine Histogram Windows--//
    //Truth
    h_truth_trhit_tim = new TH1F("h_truth_trhit_tim", Form("Simulated Tracker Hits Time of Arriaval in %s; Time of Arrival [ns]", layer.c_str()), bins, minTim, maxTim);
    h_truth_trhit_edp = new TH1F("h_truth_trhit_edp", Form("Simulated Tracker Hits Energy Deposited in %s; Energy Deposited [GeV]", layer.c_str()), bins, minEn, maxEn);
    //Digi
    h_digi_trhit_tim = new TH1F("h_digi_trhit_tim", Form("Digitized Tracker Hits Time of Arriaval in %s; Time of Arrival [ns]", layer.c_str()), bins, minTim, maxTim); 
    h_digi_trhit_edp = new TH1F("h_digi_trhit_edp", Form("Digitized Tracker Hits Energy Deposited in %s; Energy Deposited [GeV]", layer.c_str()), bins, minEn, maxEn);
    //Reco
    h_reco_trhit_tim = new TH1F("h_reco_trhit_tim", Form("Reconstructed Tracker Hits Time of Arriaval in %s; Time of Arrival [ns]", layer.c_str()), bins, minTim, maxTim); 
    h_reco_trhit_edp = new TH1F("h_reco_trhit_edp", Form("Reconstructed Tracker Hits Energy Deposited in %s; Energy Deposited [GeV]", layer.c_str()), bins, minEn, maxEn);

    h_diff_edp = new TH1F("h_diff_edp", "EDP (Reco - Digi) Difference", bins, minEn, maxEn);
    h_diff_tim = new TH1F("h_diff_tim", "Time (Reco - Digi) Difference", bins, minTim, maxTim);


    //--Harvest the leaves from the braches--//
    
    //--Simulated Tracker Hit: SINGAL--//
    int n_simHits;
    float *simHits_edp = new float[1500000];
    float *simHits_tim = new float[1500000];

    myLCTuple_truthReco->SetBranchAddress("nsth", &n_simHits);
    myLCTuple_truthReco->SetBranchAddress("stedp", simHits_edp);
    myLCTuple_truthReco->SetBranchAddress("sttim", simHits_tim);

    //--Digitized Tracker Hit: SIGNAL --//
    int n_digHits;
    float *digHits_edp = new float[1500000];
    float *digHits_tim = new float[1500000];

    myLCTuple_digi->SetBranchAddress("ntrh", &n_digHits);
    myLCTuple_digi->SetBranchAddress("thedp", digHits_edp);
    myLCTuple_digi->SetBranchAddress("thtim", digHits_tim);

    //--Reconstructed Tracker Hit: SIGNAL --//
    int n_recHits;
    float *recHits_edp = new float[1500000];
    float *recHits_tim = new float[1500000];

    myLCTuple_truthReco->SetBranchAddress("ntrh", &n_recHits);
    myLCTuple_truthReco->SetBranchAddress("thedp", recHits_edp);
    myLCTuple_truthReco->SetBranchAddress("thtim", recHits_tim);

    const long int nEntries_tr = ( maxEvents < 0 ? myLCTuple_truthReco->GetEntries() : maxEvents );
    for(int ientry=0; ientry<nEntries_tr; ientry++){
        myLCTuple_truthReco->GetEntry(ientry);
        //cout << "number of simHits: " << n_simHits << endl;
        
        for(int iTruth = 0; iTruth<n_simHits; iTruth++){
            //cout << "Loop iteration number: " << iTruth << endl;
            h_truth_trhit_edp->Fill(simHits_edp[iTruth]);
            h_truth_trhit_tim->Fill(simHits_tim[iTruth]);
        }

        for(int iReco = 0; iReco<n_recHits; iReco++){
            h_reco_trhit_edp->Fill(recHits_edp[iReco]);
            h_reco_trhit_tim->Fill(recHits_tim[iReco]);
        }


        if ( ientry % 100 == 0 ) std::cout << ientry << " / " << nEntries_tr << endl;
    
    }

    const long int nEntries_d = ( maxEvents < 0 ? myLCTuple_digi->GetEntries() : maxEvents );
    for(int ientry=0; ientry<nEntries_d; ientry++){
        myLCTuple_digi->GetEntry(ientry);
        //cout << "number of simHits: " << n_simHits << endl;
        
        for(int iDigi = 0; iDigi<n_digHits; iDigi++){
            h_digi_trhit_edp->Fill(digHits_edp[iDigi]);
            h_digi_trhit_tim->Fill(digHits_tim[iDigi]);
        }

        if ( ientry % 100 == 0 ) std::cout << ientry << " / " << nEntries_d << endl;
    
    }

    //Plot the time overlap for truth, digi, and reco
    plot_three_overlapped_hists(h_truth_trhit_tim, h_reco_trhit_tim, h_digi_trhit_tim, dirname, layer, Form("%s Time of Arrival: Truth vs Digi vs Reco Tracker Hits in ", keyWord.c_str()), "Time [ns]", "Events", "Truth Hits", "Digitized Hits", "Reconstructed Hits", Form("time_%s",keyWord.c_str()));

    //Plot the time overlap for truth, digi, and reco
    plot_three_overlapped_hists(h_truth_trhit_edp, h_reco_trhit_edp, h_digi_trhit_edp, dirname, layer, Form("%s Energy Deposited: Truth vs Digi vs Reco Tracker Hits in ", keyWord.c_str()), "Energy Deposited [GeV]", "Events", "Truth Hits", "Digitized Hits", "Reconstructed Hits", Form("edp_%s",keyWord.c_str()));

    //plot edp and tim diff
    subtract_hists(h_reco_trhit_edp, h_digi_trhit_edp, "EDP Difference in Reco and Digi Hits", "Energy Deposited [GeV]", dirname,"edp_diff");
    subtract_hists(h_reco_trhit_tim, h_digi_trhit_tim, "Time Difference in Reco and Digi Hits", "Time [ns]", dirname,"tim_diff");

}
