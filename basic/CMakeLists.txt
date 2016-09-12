# basic/CMakeLists.txt
# Brown Basic Libraries.

project(basic)

include_directories( ${lems_SOURCE_DIR}/basic )
include_directories( ${lems_SOURCE_DIR}/db3p )
include_directories( ${basic_BINARY_DIR} )

# The Development Brown Process Library
subdirs( dbpro )

# The Development Brown Numerical Library
subdirs( dbnl )

# The Development Brown Geometry Library
subdirs( dbgl ) 

# The Development Brown Graph/Hypergraph Library
subdirs( dbgrl ) 

# The Development Brown Spatial Objects Library
subdirs( dbsol ) 

# The Development Brown Spatial Indexing Library
subdirs( dbspi )

# The Development Brown Image Library
subdirs( dbil ) 

# The Development Brown Image3d Library
subdirs( dbil3d ) 

# The Development Brown Video Library
subdirs( dbvidl )

# The Development Brown Video Streaming Library
subdirs( dbvidl2 ) 

# Development Brown GUI (stuff that could move to /brl/bbas/bgui)
subdirs( dbgui )

# Development XML Parser using Xerces
#subdirs( dbxml-xerces )

# Development XML Parser - augmenting bxml
subdirs( dbxml )

# Development XML Parser using expat
# NOTE: moved to vxl/contrib/brl/bbas/bxml
##subdirs( dbxml2 )

# The Development Brown Statistics Library
subdirs( dbsta )

# The Development Brown Utility Library
subdirs( dbul )

# The Development Brown Dynamic Programming Library
subdirs( dbdp ) 

# The Development Brown Basic (templates) Library
subdirs( dbbl ) 

# The Development Brown Testing Library
subdirs( dbtest ) 

