#include "assert.h"
#include "ifx.hh"

namespace sisalc {

   NODE node::null(0,true); // Node locked to null

   void node::self(SP<node> p) { assert(p.get()==this); mSelf = p; }
   SP<node> node::self() const { assert(mSelf.get() == this); return mSelf; }

   void node::writeSelf(ostream& os) const {
      assert(valid());

      IFObject::writeSelf(os);
      for(int port=1; port <= mInputs.size(); ++port) {
         os << *mInputs[port-1];
      }
   }

   node::~node() {
      cerr << "Destroy node @ " << this  
           << " with parent " << *mParent << endl; 
   }

   node::node() 
      : mOpCode(999) {
   }

   node::node(unsigned opCode)
      : mOpCode(opCode) {
      assert(valid());
   }

   node::node(const char* s)
   {
      mOpCode = lookup(s);
   }

   void node::setParent(SP<graph> parent) {
      mParent = parent;
   }

   unsigned int node::lookup(const char* s) {
      const operation_t* operations = getTable();
      for( ; operations && operations->name; ++operations) {
         //cerr << "Compare " << s << " to " << operations->name << endl;
         if ( strcmp(s,operations->name) == 0 ) {
            return operations->opCode;
         }
      }
      throw "unknown opcode";
   }

   const char* node::name() const {
      const operation_t* operations = getTable();
      for( ; operations && operations->name; ++operations) {
         if ( mOpCode == operations->opCode ) {
            return operations->name;
         }
      }
      return 0;
   }
   
   bool node::valid() const { return name()?true:false; }

   unsigned int node::label() const {
      assert(mParent.get());
      return mParent->offset(self());
   }
   int node::i1() const {return label();}
   int node::i2() const {return mOpCode;}

   void node::attachInput(EDGE E, int port) {
      assert(E.get());
      assert(port > 0);

      // Make sure the edge points to node
      E->setDestination(self(),port);

      if ( mInputs.size() < port ) mInputs.resize(port);
      // Must be empty before attaching edge
      assert(mInputs[port-1].get() == 0);
      mInputs[port-1] = E;
   }

   void node::attachOutput(EDGE E, int port) {
      assert(E.get());
      assert(port > 0);

      cerr << "In attach output\n";
      cerr << "My label is " << self()->label() << endl;
      E->setSource(self(),port);
      if ( mOutputs.size() < port ) mOutputs.resize(port);
      mOutputs[port-1].push_back(E);
   }

   static node::operation_t definitions[] = {
      {"AAddH",100},
         {"AAddL",101},
            {"AAdjust",102},
               {"ABuild",103},
                  {"ACatenate",104},
                     {"AElement",105},
                        {"AFill",106},
                           {"AGather",107},
                              {"AIsEmpty",108},
                                 {"ALimH",109},
                                    {"ALimL",110},
                                       {"ARemH",111},
                                          {"ARemL",112},
                                             {"AReplace",113},
                                                {"AScatter",114},
                                                   {"ASetL",115},
                                                      {"ASize",116},
                                                         {"Abs",117},
                                                            {"BindArguments",118},
                                                               {"Bool",119},
                                                                  {"Call",120},
                                                                     {"Char",121},
                                                                        {"Div",122},
                                                                           {"Double",123},
                                                                              {"Equal",124},
                                                                                 {"Exp",125},
                                                                                    {"FirstValue",126},
                                                                                       {"FinalValue",127},
                                                                                          {"Floor",128},
                                                                                             {"Int",129},
                                                                                                {"IsError",130},
                                                                                                   {"Less",131},
                                                                                                      {"LessEqual",132},
                                                                                                         {"Max",133},
                                                                                                            {"Min",134},
                                                                                                               {"Minus",135},
                                                                                                                  {"Mod",136},
                                                                                                                     {"Neg",137},
                                                                                                                        {"NoOp",138},
                                                                                                                           {"Not",139},
                                                                                                                              {"NotEqual",140},
                                                                                                                                 {"Plus",141},
                                                                                                                                    {"RangeGenerate",142},
                                                                                                                                       {"RBuild",143},
                                                                                                                                          {"RElements",144},
                                                                                                                                             {"RReplace",145},
                                                                                                                                                {"RedLeft",146},
                                                                                                                                                   {"RedRight",147},
                                                                                                                                                      {"RedTree",148},
                                                                                                                                                         {"Reduce",149},
                                                                                                                                                            {"RestValues",150},
                                                                                                                                                               {"Single",151},
                                                                                                                                                                  {"Times",152},
                                                                                                                                                                     {"Trunc",153},
                                                                                                                                                                        {"PrefixSize",154},
                                                                                                                                                                           {"Error",155},
                                                                                                                                                                              {"ReplaceMulti",156},
                                                                                                                                                                                 {"Convert",157},
                                                                                                                                                                                    {"CallForeign",158},
                                                                                                                                                                                       {"AElementN",159},
                                                                                                                                                                                          {"AElementP",160},
                                                                                                                                                                                             {"AElementM",161},
                                                                                                                                                                                                {"AAddLAT",170},
                                                                                                                                                                                                   {"AAddHAT",171},
                                                                                                                                                                                                      {"ABufPartition",172},
                                                                                                                                                                                                         {"ABuildAT",173},
                                                                                                                                                                                                            {"ABufScatter",174},
                                                                                                                                                                                                               {"ACatenateAT",175},
                                                                                                                                                                                                                  {"AElementAT",176},
                                                                                                                                                                                                                     {"AExtractAT",177},
                                                                                                                                                                                                                        {"AFillAT",178},
                                                                                                                                                                                                                           {"AGatherAT",179},
                                                                                                                                                                                                                              {"ARemHAT",180},
                                                                                                                                                                                                                                 {"ARemLAT",181},
                                                                                                                                                                                                                                    {"AReplaceAT",182},
                                                                                                                                                                                                                                       {"ArrayToBuf",183},
                                                                                                                                                                                                                                          {"ASetLAT",184},
                                                                                                                                                                                                                                             {"DefArrayBuf",185},
                                                                                                                                                                                                                                                {"DefRecordBuf",186},
                                                                                                                                                                                                                                                   {"FinalValueAT",187},
                                                                                                                                                                                                                                                      {"MemAlloc",188},
                                                                                                                                                                                                                                                         {"BufElements",189},
                                                                                                                                                                                                                                                            {"RBuildAT",190},
                                                                                                                                                                                                                                                               {"RecordToBuf",191},
                                                                                                                                                                                                                                                                  {"RElementsAT",192},
                                                                                                                                                                                                                                                                     {"ReduceAT",193},
                                                                                                                                                                                                                                                                        {"ShiftBuffer",194},
                                                                                                                                                                                                                                                                           {"ScatterBufPartitions",195},
                                                                                                                                                                                                                                                                              {"RedLeftAT",196},
                                                                                                                                                                                                                                                                                 {"RedRightAT",197},
                                                                                                                                                                                                                                                                                    {"RedTreeAT",198},
                                                                                                                                                                                                                                                                                       {"RESERVED",199},
                                                                                                                                                                                                                                                                                          {"LoopPoolEnq",200},
                                                                                                                                                                                                                                                                                             {"ReadyListEnq",201},
                                                                                                                                                                                                                                                                                                {"Assign",202},
                                                                                                                                                                                                                                                                                                   {"GetArrayBase",203},
                                                                                                                                                                                                                                                                                                      {"OptAElement",204},
                                                                                                                                                                                                                                                                                                         {"Div2",205},
                                                                                                                                                                                                                                                                                                            {"UTagTest",206},
                                                                                                                                                                                                                                                                                                               {"UBuild",207},
                                                                                                                                                                                                                                                                                                                  {"UGetTag",208},
                                                                                                                                                                                                                                                                                                                     {"UElement",209},
                                                                                                                                                                                                                                                                                                                        {"OptLoopPoolEnq",210},
                                                                                                                                                                                                                                                                                                                           {"BuildSlices",211},
                                                                                                                                                                                                                                                                                                                              {"AIndexMinus",220},
                                                                                                                                                                                                                                                                                                                                 {"AIndexPlus",221},
                                                                                                                                                                                                                                                                                                                                    {"FirstSum",222},
                                                                                                                                                                                                                                                                                                                                       {"FirstMin",223},
                                                                                                                                                                                                                                                                                                                                          {"FirstMax",224},
                                                                                                                                                                                                                                                                                                                                             {"Tri",225},
                                                                                                                                                                                                                                                                                                                                                {"FirstAbsMin",226},
                                                                                                                                                                                                                                                                                                                                                   {"FirstAbsMax",227},
                                                                                                                                                                                                                                                                                                                                                      {"AStore",250},
                                                                                                                                                                                                                                                                                                                                                         {"Least",251},
                                                                                                                                                                                                                                                                                                                                                            {"Greatest",252},
                                                                                                                                                                                                                                                                                                                                                               {"Product",253},
                                                                                                                                                                                                                                                                                                                                                                  {"Sum",254},
                                                                                                                                                                                                                                                                                                                                                                     {"Great",260},
                                                                                                                                                                                                                                                                                                                                                                        {"GreatEqual",261},
                                                                                                                                                                                                                                                                                                                                                                           {"BRBuild",262},
                                                                                                                                                                                                                                                                                                                                                                              {"BRElements",263},
                                                                                                                                                                                                                                                                                                                                                                                 {"BRReplace",264},
                                                                                                                                                                                                                                                                                                                                                                                    {"SaveCallParam",266},
                                                                                                                                                                                                                                                                                                                                                                                       {"SaveSliceParam",267},
                                                                                                                                                                                                                                                                                                                                                                                          {"BuildLoopSlices",268},
                                                                                                                                                                                                                                                                                                                                                                                             {"OptAReplace",269},
                                                                                                                                                                                                                                                                                                                                                                                                {"MemAllocDVI",299},
                                                                                                                                                                                                                                                                                                                                                                                                   {"MemAllocDV",300},
                                                                                                                                                                                                                                                                                                                                                                                                      {"AAddHATDVI",301},
                                                                                                                                                                                                                                                                                                                                                                                                         {"AAddHATDV",302},
                                                                                                                                                                                                                                                                                                                                                                                                            {"AAddLATDVI",303},
                                                                                                                                                                                                                                                                                                                                                                                                               {"AAddLATDV",304},
                                                                                                                                                                                                                                                                                                                                                                                                                  {"ACatenateATDVI",305},
                                                                                                                                                                                                                                                                                                                                                                                                                     {"ACatenateATDV",306},
                                                                                                                                                                                                                                                                                                                                                                                                                        {"ABuildATDVI",307},
                                                                                                                                                                                                                                                                                                                                                                                                                           {"ABuildATDV",308},
                                                                                                                                                                                                                                                                                                                                                                                                                              {"ReduceATDVI",309},
                                                                                                                                                                                                                                                                                                                                                                                                                                 {"ReduceATDV",310},
                                                                                                                                                                                                                                                                                                                                                                                                                                    {"AGatherATDVI",311},
                                                                                                                                                                                                                                                                                                                                                                                                                                       {"AGatherATDV",312},
                                                                                                                                                                                                                                                                                                                                                                                                                                          {"OptNoOp",313},
                                                                                                                                                                                                                                                                                                                                                                                                                                             {"Peek",314},
                                                                                                                                                                                                                                                                                                                                                                                                                                                {"Ragged",315},
                                                                                                                                                                                                                                                                                                                                                                                                                                                   {"PSAllocCond",316},
                                                                                                                                                                                                                                                                                                                                                                                                                                                      {"PSAllocOne",317},
                                                                                                                                                                                                                                                                                                                                                                                                                                                         {"PSAllocTwo",318},
                                                                                                                                                                                                                                                                                                                                                                                                                                                            {"PSAllocSpare1",319},
                                                                                                                                                                                                                                                                                                                                                                                                                                                               {"PSAllocSpare2",320},
                                                                                                                                                                                                                                                                                                                                                                                                                                                                  {"PSFreeOne",321},
                                                                                                                                                                                                                                                                                                                                                                                                                                                                     {"PSFreeTwo",322},
                                                                                                                                                                                                                                                                                                                                                                                                                                                                        {"PSFreeSpare1",323},
                                                                                                                                                                                                                                                                                                                                                                                                                                                                           {"PSFreeSpare2",324},
                                                                                                                                                                                                                                                                                                                                                                                                                                                                              {"PSManager",325},
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 {"PSManagerSwap",326},
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    {"PSScatter",327},
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       {"PSMemAllocDVI",328},
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          {"Spawn",330},
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             {"Rat",331},
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                {"Wat",332},
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   {"SStep",333},
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      {"SAddH",334},
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         {"SBuild",335},
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            {"SCatenate",336},
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               {"SGather",337},
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  {"SIsEmpty",338},
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     {"SScatter",339},
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        {"SSize",340},
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           {"SPrefixSize",341},
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              {"AReplaceRange",400},
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 {"ASelectRange",401},
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    {"AReplaceMulti",402},
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       {"Reduction",403},
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          {"Dot",404},
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             {"Cross",405},
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                {"Assert",406},
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   {"Require",407},
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      {0,0}
   };

   const node::operation_t* node::getTable() const { return definitions; }
   
}
