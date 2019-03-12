#include "ADIOS1DataAdaptor.h"


#include "MeshMetadata.h"
#include "Partitioner.h"
#include "BlockPartitioner.h"
#include "Error.h"
#include "Timer.h"
#include "ADIOS1Schema.h"
#include "VTKUtils.h"

#include <vtkCompositeDataIterator.h>
#include <vtkDataSetAttributes.h>
#include <vtkInformation.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkObjectFactory.h>
#include <vtkSmartPointer.h>
#include <vtkDataSet.h>

#include <sstream>

namespace sensei
{
struct ADIOS1DataAdaptor::InternalsType
{
  InternalsType() : Stream() {}

  senseiADIOS1::InputStream Stream;
  senseiADIOS1::DataObjectCollectionSchema Schema;
};

//----------------------------------------------------------------------------
senseiNewMacro(ADIOS1DataAdaptor);

//----------------------------------------------------------------------------
ADIOS1DataAdaptor::ADIOS1DataAdaptor() : Internals(nullptr)
{
  this->Internals = new InternalsType;
}

//----------------------------------------------------------------------------
ADIOS1DataAdaptor::~ADIOS1DataAdaptor()
{
  delete this->Internals;
}

//----------------------------------------------------------------------------
int ADIOS1DataAdaptor::SetFileName(const std::string &fileName)
{
  this->Internals->Stream.FileName = fileName;
  return 0;
}

//----------------------------------------------------------------------------
int ADIOS1DataAdaptor::SetReadMethod(const std::string &method)
{
  return this->Internals->Stream.SetReadMethod(method);
}

//----------------------------------------------------------------------------
int ADIOS1DataAdaptor::SetReadMethod(ADIOS_READ_METHOD method)
{
  this->Internals->Stream.ReadMethod = method;
  return 0;
}

//----------------------------------------------------------------------------
int ADIOS1DataAdaptor::Initialize(pugi::xml_node &node)
{
  timer::MarkEvent mark("ADIOS1DataAdaptor::Initialize");

  this->InTransitDataAdaptor::Initialize(node);

  // TODO -- what other params should I grab?
  // file name? read method? data requirements?

  return 0;
}

//----------------------------------------------------------------------------
int ADIOS1DataAdaptor::Finalize()
{
  timer::MarkEvent mark("ADIOS1DataAdaptor::Finalize");
  return 0;
}

//----------------------------------------------------------------------------
int ADIOS1DataAdaptor::OpenStream()
{
  timer::MarkEvent mark("ADIOS1DataAdaptor::OpenStream");

  if (this->Internals->Stream.Open(this->GetCommunicator()))
    {
    SENSEI_ERROR("Failed to open stream")
    return -1;
    }

  // initialize the time step
  if (this->UpdateTimeStep())
    return -1;

  return 0;
}

//----------------------------------------------------------------------------
int ADIOS1DataAdaptor::StreamGood()
{
  return this->Internals->Stream.Good();
}

//----------------------------------------------------------------------------
int ADIOS1DataAdaptor::CloseStream()
{
  timer::MarkEvent mark("ADIOS1DataAdaptor::CloseStream");

  this->Internals->Stream.Close();

  return 0;
}

//----------------------------------------------------------------------------
int ADIOS1DataAdaptor::AdvanceStream()
{
  timer::MarkEvent mark("ADIOS1DataAdaptor::AdvanceStream");

  if (this->Internals->Stream.AdvanceTimeStep())
    return -1;

  if (this->UpdateTimeStep())
    return -1;

  return 0;
}

//----------------------------------------------------------------------------
int ADIOS1DataAdaptor::UpdateTimeStep()
{
  timer::MarkEvent mark("ADIOS1DataAdaptor::UpdateTimeStep");

  // update data object time and time step
  unsigned long timeStep = 0;
  double time = 0.0;

  if (this->Internals->Schema.ReadTimeStep(this->GetCommunicator(),
    this->Internals->Stream, timeStep, time))
    {
    SENSEI_ERROR("Failed to update time step")
    return -1;
    }

  this->SetDataTimeStep(timeStep);
  this->SetDataTime(time);

  // read metadata
  if (this->Internals->Schema.ReadMeshMetadata(this->GetCommunicator(),
    this->Internals->Stream))
    {
    SENSEI_ERROR("Failed to read metadata")
    return -1;
    }

  // set up the name-object map
  unsigned int nMeshes = 0;
  if (this->Internals->Schema.GetNumberOfObjects(nMeshes))
    {
    SENSEI_ERROR("Failed to get the number of meshes")
    return -1;
    }

  return 0;
}

//----------------------------------------------------------------------------
int ADIOS1DataAdaptor::GetSenderMeshMetadata(unsigned int id,
  MeshMetadataPtr &metadata)
{
  if (this->Internals->Schema.GetMeshMetadata(id, metadata))
    {
    SENSEI_ERROR("Failed to get metadata for object " << id)
    return -1;
    }

  return 0;
}

//----------------------------------------------------------------------------
int ADIOS1DataAdaptor::GetNumberOfMeshes(unsigned int &numMeshes)
{
  numMeshes = 0;
  if (this->Internals->Schema.GetNumberOfObjects(numMeshes))
    return -1;
  return  0;
}

//----------------------------------------------------------------------------
int ADIOS1DataAdaptor::GetMeshMetadata(unsigned int id, MeshMetadataPtr &metadata)
{
  // check if some uber analysis told us how the data should land by
  // passing in reciever metadata
  if (this->GetReceiverMeshMetadata(id, metadata))
    {
    // none set, we'll use the partitioner to figure it out
    // first take a look at what's available
    MeshMetadataPtr senderMd;
    if (this->GetSenderMeshMetadata(id, senderMd))
      {
      SENSEI_ERROR("Failed to get sender metadata")
      return -1;
      }

    // get the partitioner, default to the block based layout
    Partitioner *part = this->GetPartitioner();
    if (!part)
      {
      part = new BlockPartitioner();
      }

    MeshMetadataPtr recverMd;
    if (part->GetPartition(this->GetCommunicator(), senderMd, recverMd))
      {
      SENSEI_ERROR("Failed to determine a suitable layout to receive the data")
      this->CloseStream();
      }

    metadata = recverMd;
    }

  return 0;
}

//----------------------------------------------------------------------------
int ADIOS1DataAdaptor::GetMesh(const std::string &meshName,
   bool structureOnly, vtkDataObject *&mesh)
{
  timer::MarkEvent mark("ADIOS1DataAdaptor::GetMesh");

  mesh = nullptr;

  // other wise we need to read the mesh at the current time step
  if (this->Internals->Schema.ReadObject(this->GetCommunicator(),
    this->Internals->Stream, meshName, mesh, structureOnly))
    {
    SENSEI_ERROR("Failed to read mesh \"" << meshName << "\"")
    return -1;
    }

  return 0;
}

//----------------------------------------------------------------------------
int ADIOS1DataAdaptor::AddGhostNodesArray(vtkDataObject *mesh,
  const std::string &meshName)
{
  return AddArray(mesh, meshName, vtkDataObject::POINT, "vtkGhostType");
}

//----------------------------------------------------------------------------
int ADIOS1DataAdaptor::AddGhostCellsArray(vtkDataObject *mesh,
  const std::string &meshName)
{
  return AddArray(mesh, meshName, vtkDataObject::CELL, "vtkGhostType");
}

//----------------------------------------------------------------------------
int ADIOS1DataAdaptor::AddArray(vtkDataObject* mesh,
  const std::string &meshName, int association, const std::string& arrayName)
{
  timer::MarkEvent mark("ADIOS1DataAdaptor::AddArray");

  // the mesh should never be null. there must have been an error
  // upstream.
  if (!mesh)
    {
    SENSEI_ERROR("Invalid mesh object")
    return -1;
    }

  if (this->Internals->Schema.ReadArray(this->GetCommunicator(),
    this->Internals->Stream, meshName, association, arrayName, mesh))
    {
    SENSEI_ERROR("Failed to read " << VTKUtils::GetAttributesName(association)
      << " data array \"" << arrayName << "\" from mesh \"" << meshName << "\"")
    return -1;
    }

  return 0;
}

//----------------------------------------------------------------------------
int ADIOS1DataAdaptor::ReleaseData()
{
  timer::MarkEvent mark("ADIOS1DataAdaptor::ReleaseData");
  return 0;
}

//----------------------------------------------------------------------------
void ADIOS1DataAdaptor::PrintSelf(ostream& os, vtkIndent indent)
{
  this->sensei::DataAdaptor::PrintSelf(os, indent);
}

}
