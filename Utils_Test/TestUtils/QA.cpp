#include "QA.hh"
#include "Out.hh"
#include <QUtils/Output/MultiStream.h>
#include <fstream>

//static QA::_ostream _out;

std::ostream QA::out(nullptr);

static QUtils::Output::MultiBuf* mb = nullptr;
static std::ofstream* file = nullptr;

void QA::Out_Init()
{
	if (mb)
	{
		return;
	}
	mb = new QUtils::Output::MultiBuf();
	file = new std::ofstream("QA_Out.txt");
	mb->push(file->rdbuf());
	mb->push(std::cout.rdbuf());
	out.rdbuf(mb);
	//std::clog.rdbuf(nullptr);
	//std::cout.rdbuf(nullptr);
	//out.rdbuf(std::cerr.rdbuf());
	//std::cerr.rdbuf(nullptr);
}