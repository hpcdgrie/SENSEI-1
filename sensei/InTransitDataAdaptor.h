#ifndef sensei_InTransitDataAdaptor_h
#define sensei_InTransitDataAdaptor_h

#include "DataAdaptor.h"

#include <vector>
#include <string>
#include <mpi.h>
#include <memory>

#include <pugixml.hpp>

class vtkAbstractArray;
class vtkDataObject;
class vtkInformation;
class vtkInformationIntegerKey;

namespace sensei
{


/// @class InTransitDataAdaptor
/// @brief InTransitDataAdaptor is an abstract base class that defines the data interface.
///
/// InTransitDataAdaptor defines the data interface. Any simulation code that interfaces with
/// Sensei needs to provide an implementation for this interface. Analysis routines
/// (via AnalysisAdator) use the InTransitDataAdaptor implementation to access simulation data.
class InTransitDataAdaptor : public sensei::DataAdaptor
{
public:
  senseiBaseTypeMacro(InTransitDataAdaptor, sensei::DataAdaptor);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  // New API that enables run-time user based control of how data lands for
  // sensei::AnalysisAdaptor's which do not need explicit control. The specific
  // transport layers will implement this, and may support different options.
  // However, they will all support the 'partitioner' attribute and the following
  // partitioning modes:
  //
  //     block  The block distribution method will distribute blocks to a rank
  //            such that consecutive blocks share a rank.
  //
  //     cyclic The cyclic distribution method will distribute blocks to a rank
  //            such that consecutive blocks are distributed over consecutive
  //            ranks (in a round-robin fashion).
  //
  //     plane  The  blocks are distributed in blocks of a specified size.
  //            The size is specified in the 'plane_size' attribute.
  //
  //     mapped The mapped method of distribution will allocate blocks
  //            in-order as listed in a nested 'proc' and 'block' elements.
  //            each entry in the block element has a corresponding entry
  //            in the proc element naming the mpi rank where the block
  //            lands
  //
  // Note, that these are core partitioning supported in SENSEI 3, specific
  // InTransitDataAdaptor instances are free to support other partitionings
  // but not required to do so.
  //
  // Illustrative example of the XML:
  //
  // <analysis type="histogram" ... >
  //
  //   <data_adaptor type="adios" partitioner="block" ... >
  //     ...
  //   </data_adaptor>
  //
  //   ...
  //
  // </analysis>
  //
  // For more information on the 'analysis element' see sensei::ConfigurableAnalysis.
  // For more information on the 'data_adaptor' 'type' attribute see
  // sensei::InTransitAdaptorFactory
  virtual int Initialize(pugi::xml_node parent) = 0;

  // New API that enables one to peak at how the data is partitioned on the
  // simulation/remote side. Analyses that need control over how data lands
  // can use this to see what data is available, associated metadata such as
  // block bounds and array metadata and how it's laid out on the sender side.
  virtual int GetSenderMeshMetadata(unsigned int id, MeshMetadataPtr &metadata);

  // New API that enables one to specify how the data is partitioned on the
  // analysis/local side. Analyses that need control over how data lands
  // can use this to say where data lands. The metadata object passed here
  // will be returned to the Analysis, and the transport layer will use it
  // to move blocks onto the correct ranks. Care, should be taken as there
  // will be variablility in terms of what various transport layers support.
  // The requirement for SENSEI 3.0 is that blocks are elemental. In other
  // words given M ranks and P blocks on the sender/simulation side, a partitioning
  // with N ranks and P blocks on the receiver/analysis side is supported.
  // A transport may support more sophistocated partitioning, but it's not
  // required. An analysis need not use this API, in that case the default
  // is handled by the transport layer. See comments in InTransitDataAdaptor::Initialize
  // for the universal partioning options as well as comments in the specific
  // transport's implementation.
  virtual int SetReceiverMeshMetadata(unsigned int id, MeshMetadataPtr metadata);

  // Core sensei::DataAdaptor API. These methods are used by the
  // sensei::AnalysisAdaptor's which do not need explicit control of where data
  // lands to access data and meta data. For these Analyses the transport layer
  // makes desicions about how data lands. The user can influence how the data
  // lands if desired via XML. See comments in InTransitDataAdaptor::Initialize
  // for the universal partioning options as well as comments in the specific
  // transport's implementation.
  virtual int GetNumberOfMeshes(unsigned int &numMeshes) = 0;

  virtual int GetMeshMetadata(unsigned int id, MeshMetadataPtr &metadata) = 0;

  int GetMeshMetadata(const std::string &meshName, MeshMetadataPtr &metadata);

  virtual int GetMesh(const std::string &meshName, bool structureOnly,
    vtkDataObject *&mesh) = 0;

  virtual int AddGhostNodesArray(vtkDataObject* mesh, const std::string &meshName);
  virtual int AddGhostCellsArray(vtkDataObject* mesh, const std::string &meshName);

  virtual int AddArray(vtkDataObject* mesh, const std::string &meshName,
    int association, const std::string &arrayName) = 0;
  virtual int AddArrays(vtkDataObject* mesh, const std::string &meshName,
    int association, const std::vector<std::string> &arrayName);
  virtual int ReleaseData() = 0;

protected:
  InTransitDataAdaptor();
  ~InTransitDataAdaptor();

  InTransitDataAdaptor(const InTransitDataAdaptor&) = delete;
  void operator=(const InTransitDataAdaptor&) = delete;

  struct InternalsType;
  InternalsType *Internals;
};

}
#endif
