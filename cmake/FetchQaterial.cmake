include(FetchContent)

set(QATERIAL_REPOSITORY "https://github.com/ahmedyarub/Qaterial.git" CACHE STRING "Repository of Qaterial")
set(QATERIAL_TAG "master" CACHE STRING "Git Tag of Qaterial")

set(QATERIAL_ENABLE_PCH FALSE CACHE INTERNAL "QATERIAL_ENABLE_PCH")
set(QATERIAL_ENABLE_TESTS FALSE CACHE INTERNAL "QATERIAL_ENABLE_TESTS")
set(QATERIAL_ENABLE_HIGHDPIFIX FALSE CACHE INTERNAL "QATERIAL_ENABLE_HIGHDPIFIX")
set(OLM_REPOSITORY https://github.com/ahmedyarub/QOlm.git CACHE INTERNAL "OLM_REPOSITORY")

FetchContent_Declare(
        qaterial
        GIT_REPOSITORY ${QATERIAL_REPOSITORY}
        GIT_TAG ${QATERIAL_TAG}
)

set(QATERIAL_FOLDER_PREFIX "Dependencies" CACHE STRING "")
FetchContent_MakeAvailable(qaterial)