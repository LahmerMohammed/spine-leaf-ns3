build_lib(
        LIBNAME opengym
        SOURCE_FILES
        helper/opengym-helper.cc
        model/messages.pb.cc
        model/container.cc
        model/opengym_env.cc
        model/opengym_interface.cc
        model/spaces.cc
        HEADER_FILES
        helper/opengym-helper.h
        model/container.h
        model/messages.pb.h
        model/opengym_env.h
        model/opengym_interface.h
        model/spaces.h
        LIBRARIES_TO_LINK ${libinternet}
        ${libconfig-store} ${PROTOBUF_LIBRARY} zmq
)
