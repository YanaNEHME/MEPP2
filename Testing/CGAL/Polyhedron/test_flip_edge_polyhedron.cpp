#include <CGAL/Cartesian.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/Polyhedron_items_with_id_3.h>

// Graph traits adaptors
#include <CGAL/boost/graph/graph_traits_Polyhedron_3.h>
#include "FEVV/Wrappings/Geometry_traits_cgal_polyhedron_3.h" // Geometry adaptor
#include "FEVV/Wrappings/properties_polyhedron_3.h"

#define USE_GENERIC_READER_WRITER // when activated you need to link to vtk
                                  // libs if VTK is used
#ifdef USE_GENERIC_READER_WRITER
#include "FEVV/Wrappings/Graph_traits_extension_cgal_polyhedron_3.h"
#include "FEVV/Filters/Generic/generic_reader.hpp"
#include "FEVV/Filters/Generic/generic_writer.hpp"
#else
#include <CGAL/IO/Polyhedron_iostream.h>
#endif

#include <fstream>
#include <string> // std::stoi

#include "Testing/Utils/utils_retrieve_halfedge.h"
#include "Testing/Utils/utils_are_meshes_identical.h"
#include "FEVV/Operators/Generic/Manifold/flip_edge.hpp"

using namespace FEVV;
using namespace FEVV::Operators;

void
test_split_edge_polyhedron(std::string input_file_path,
                           int source_index,
                           int target_index,
                           const std::string &output_file_name)
{
  typedef CGAL::Cartesian< double > Kernel;
  typedef CGAL::Polyhedron_3< Kernel, CGAL::Polyhedron_items_with_id_3 >
      Polyhedron;

  typedef boost::graph_traits< Polyhedron > GraphTraits;
  typedef typename GraphTraits::halfedge_descriptor halfedge_descriptor;

  //---------------------------------------------------------------------
  // Load a mesh
  Polyhedron p;
#ifdef USE_GENERIC_READER_WRITER
  FEVV::PMapsContainer pmaps;
  FEVV::Filters::read_mesh(input_file_path, p, pmaps);
#else
  std::ifstream toTreat(inputFilePath);
  if(!toTreat)
  {
    std::cout << "Unable to read file " << inputFilePath << std::endl;
    exit(EXIT_FAILURE);
  }
  toTreat >> P;
  toTreat.close();
#endif

  //---------------------------------------------------------------------
  halfedge_descriptor h = retrieve_halfedge(p, source_index, target_index);
  if(h == GraphTraits::null_halfedge())
  {
    std::cout << "Failed to retrieve edge from " << source_index << " to "
              << target_index << "." << std::endl;
    std::cout << "Exiting";
    exit(EXIT_FAILURE);
  }

  std::cout << "Flipping edge " << source_index << " to " << target_index << "."
            << std::endl;
  flip_edge(p, h);
#ifdef USE_GENERIC_READER_WRITER
  FEVV::Filters::write_mesh(output_file_name, p, pmaps);
#else
  std::ofstream os(outputFileName);
  os << P;
  os.close();
#endif
}

//------------------------------------------------------------------------------
/// [Snippet Ctest Example]
int
main(int narg, char **argv)
{
  if(narg < 3 || narg > 5)
  {
    std::cout << "Usage: a.out filename_a splitting_case filename_b filename_c";
    std::cout << " - filename_a filename of file to be treated (off format)"
              << std::endl;
    std::cout << " - splitting_case case: integer specifying the edge to delete"
              << std::endl;
    std::cout << "      0 for edge 1 --> 2" << std::endl;
    std::cout << "      1 for edge 2 --> 1" << std::endl;
    std::cout << "      2 for edge 3 --> 4" << std::endl;
    std::cout << "      3 for edge 4 --> 3" << std::endl;
    std::cout
        << " - filename_b optional reference off file of output mesh result."
        << std::endl;
    std::cout << " - filename_c optional reference off file of valid result."
              << std::endl;

    exit(EXIT_FAILURE);
  }

  int splitting_case = std::stoi(std::string(argv[2]));
  std::string output_file_name = std::string(argv[3]);

  if(splitting_case == 0)
  {
    // Whatever comes first
    test_split_edge_polyhedron(argv[1], 1, 2, output_file_name);
  }
  else if(splitting_case == 1)
  {
    test_split_edge_polyhedron(argv[1], 2, 1, output_file_name);
  }
  else if(splitting_case == 2)
  {
    test_split_edge_polyhedron(argv[1], 3, 4, output_file_name);
  }
  else if(splitting_case == 3)
  {
    test_split_edge_polyhedron(argv[1], 4, 3, output_file_name);
  }
  else
  {
    std::cout << "Unknown deletation case " << argv[2] << std::endl;
  }

  if(narg == 5)
  {
    if(!are_meshes_equal(output_file_name, argv[4], false))
      return 1; // test failed
  }

  return 0;
}
/// [Snippet Ctest Example]