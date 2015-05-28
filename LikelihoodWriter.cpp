#include "LikelihoodWriter.h"
#include <stdexcept>

ProbWriter::ProbWriter(string fname, long Ntotal, long Nmap, long M) : outF(fname)
{
  // Open and initialize output file {{{
  if(!outF.is_open()){
    throw std::runtime_error("Unable to open output file " + fname);
  }
  outF<<"# Ntotal "<<Ntotal<<"\n# Nmap "<<Nmap<<"\n# M "<<M<<endl;
  outF<<"# LOGFORMAT (probabilities saved on log scale.)\n# r_name num_alignments (tr_id prob )^*{num_alignments}"<<endl;
  outF.precision(9);
  outF<<scientific;
  // }}}
}

void ProbWriter::writeRead(string name, vector<ns_parseAlignment::TagAlignment> alignments)
{
  outF<<name<<" "<<alignments.size()+1;
  double minProb = 1;
  for (long i=0; i<(long)alignments.size(); i++){
    if (minProb>alignments[i].getLowProb())
      minProb = alignments[i].getLowProb();
    outF<<" "<<alignments[i].getTrId()
	<<" "<<alignments[i].getProb();
  }
  outF<<" 0 "<<minProb<<endl;
}

void ProbWriter::writeDummy(string name)
{
  outF<<name<<" 1 0 0"<<endl;
}

void ProbWriter::finalize()
{
  outF.close();
}

//
// Class MemoryWriter
//

MemoryWriter::MemoryWriter(AlignmentLikelihoods *outalign, long Ntotal, long Nmap, long M)
{
  likelihoods = outalign;
  likelihoods->alignments->init(Nmap,0,M);
  likelihoods->Ntotal = Ntotal;
  likelihoods->Nmap = Nmap;
  likelihoods->M = M;
}

void MemoryWriter::writeRead(string name, vector<ns_parseAlignment::TagAlignment> alignments)
{
  double minProb = 1;
  for (long i=0; i<(long)alignments.size(); i++){
    if (minProb>alignments[i].getLowProb())
      minProb = alignments[i].getLowProb();
    likelihoods->alignments->pushAlignmentL(alignments[i].getTrId(),
					    alignments[i].getProb());
  }
  likelihoods->alignments->pushAlignment(0, minProb);
  likelihoods->alignments->pushRead();
}

void MemoryWriter::writeDummy(string name)
{
  likelihoods->alignments->pushAlignment(0, 0);
  likelihoods->alignments->pushRead();
}

void MemoryWriter::finalize()
{
}