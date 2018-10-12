
// #include "dogitoys.h"

#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include <pybind11/iostream.h>
#include <pybind11/stl.h>
// #include <exception>

#include "region.h"
#include "regionseq.h"
// #include "dogitoys.h"

namespace py = pybind11;
using namespace pybind11::literals;
using namespace HKL;
// using namespace DOGIToys;

using std::string;
using std::ifstream;

PYBIND11_MODULE(pyHKL, m){
    py::register_exception<RegionError>(m, "RegionError");

    py::class_<Region>(m, "Region")
        // Constructors
        .def(py::init<>())
        .def(py::init<string, int, int, string>(), "chrom"_a, "first"_a,
        "last"_a, "strand"_a = "")
        .def(py::init<string, int, string>(), "chrom"_a, "pos"_a, "strand"_a = "")
        .def(py::init<string>(), "query"_a)

        // Operators
        // .def(py::init<>())
        .def(py::self == py::self)
        .def(py::self < py::self)
        .def(py::self > py::self)
        .def(py::self <= py::self)
        .def(py::self >= py::self)

        //Setters
        .def("setChrom", &Region::setChrom, "query"_a)

        .def("setRange", py::overload_cast<int, int>(&Region::setRange),
             "first"_a.noconvert(), "last"_a.noconvert() = 0)
        .def("setRange", py::overload_cast<pair<int, int>>(&Region::setRange),
             "range"_a.noconvert())
        .def("setRange", py::overload_cast<string>(&Region::setRange),
             "coord"_a.noconvert())

        .def("setPos", &Region::setPos, "pos"_a.noconvert())

        .def("setFirst", &Region::setFirst, "first"_a.noconvert())
        .def("setLast", &Region::setLast, "last"_a.noconvert())

        .def("setStrand", py::overload_cast<string>(&Region::setStrand),
             "strand"_a)

        .def("resize", py::overload_cast<int, int, bool>(&Region::resize),
        "upstream"_a, "downstream"_a, "orient"_a = false)
        .def("resize", py::overload_cast<std::pair<int, int>, bool>(&Region::resize),
        "extent"_a, "orient"_a = false)

        //Checkers
        .def("isEmpty", &Region::isEmpty)
        .def("isPos", &Region::isPos)
        .def("isRange", &Region::isRange)
        .def("isPure", &Region::isPure)

        //Getters
        .def("getChrom", py::overload_cast<>(&Region::getChrom, py::const_))
        .def("getChrom",
             py::overload_cast<const Region&>(&Region::getChrom, py::const_),
             "other"_a)

        .def("getFirst", &Region::getFirst)
        .def("getCenter", &Region::getCenter)
        .def("getLast", &Region::getLast)
        .def("getRange", &Region::getRange)

        .def("getStrand", [](const Region& self){
            if (auto strand = self.getStrand(); strand) return string(1, strand);
            else return string();})
        .def("getStrand", [](const Region& self, const Region& other){
            if (auto strand = self.getStrand(other); strand) return string(1, strand);
            else return string();})

        .def("getLength", &Region::getLength)

        .def("at", &Region::at, "pos"_a, "orient"_a = false)
        .def("str", &Region::str)

        //Compare
        .def("sameChrom", &Region::sameChrom, "other"_a)
        .def("sharesChrom", &Region::sharesChrom, "other"_a)

        .def("sameRange", &Region::sameRange, "other"_a)
        .def("sharesRange", &Region::sharesRange, "other"_a)

        .def("sameStrand", &Region::sameStrand, "other"_a)
        .def("sharesStrand", &Region::sharesStrand, "other"_a)

        //Relationship
        .def("dist", &Region::dist, "Calculate distance between regions",
             "other"_a, "orient"_a=false)

        .def("shares", &Region::shares, "other"_a)
        .def("around", &Region::around, "other"_a)
        .def("next", &Region::next, "other"_a)
        .def("inside", &Region::inside, "other"_a)
        .def("covers", &Region::covers, "other"_a)

        .def("upstream", &Region::upstream, "other"_a, "orient"_a = true)
        .def("downstream", &Region::downstream, "other"_a, "orient"_a = true)

        //Generating Regions
        .def("getShared", &Region::getShared, "other"_a)
        .def("getUnion", &Region::getUnion, "other"_a)
        .def("getDiff", &Region::getDiff, "other"_a)
        .def("getGap", &Region::getGap, "other"_a)

        //Share Rate
        .def("getSharedLength", &Region::getSharedLength, "other"_a)
        .def("getCoveredRate", &Region::getCoveredRate, "other"_a)
        .def("getCapturedRate", &Region::getCapturedRate, "other"_a)

        //Relative positions
        .def("getPosRel",
             py::overload_cast<const Region&, int, bool>(
                 &Region::getPosRel<int>, py::const_),
             "other"_a, "pos"_a = 0, "orient"_a = false)
        .def("getPosRel",
            py::overload_cast<int, bool>(&Region::getPosRel<int>, py::const_),
            "pos"_a, "orient"_a = false)
        .def("getPosRel",
             py::overload_cast<const Region&, double, bool>(
                 &Region::getPosRel<double>, py::const_),
             "other"_a, "pos"_a = 0, "orient"_a = false)
        .def("getPosRel",
             py::overload_cast<double, bool>(
                 &Region::getPosRel<double>, py::const_),
             "pos"_a, "orient"_a = false)

        .def("getPosRelRatio",
             py::overload_cast<const Region&, int, bool>(
                 &Region::getPosRelRatio<int>, py::const_),
             "other"_a, "pos"_a = 0, "orient"_a = false)
        .def("getPosRelRatio",
             py::overload_cast<int, bool>(
                 &Region::getPosRelRatio<int>, py::const_),
             "pos"_a, "orient"_a = false)
        .def("getPosRelRatio",
             py::overload_cast<const Region&, double, bool>(
                 &Region::getPosRelRatio<double>, py::const_),
             "other"_a, "pos"_a = 0, "orient"_a = false)
        .def("getPosRelRatio",
             py::overload_cast<double, bool>(
                 &Region::getPosRelRatio<double>, py::const_),
             "pos"_a = 0, "orient"_a = false)

        //Python specific
        .def("__repr__", [](const Region& a){
            return "<HKL.Region in " + a.str() + ">";})
        .def("__str__", [](const Region& a){return a.str();})
        .def("__hash__", [](const Region& a){return std::hash<Region>{}(a);})
        .def("__len__", [](const Region& a){return a.getLength();});

    // return m.ptr();

    // py::class_<DOGI>(m, "DOGI")
    //     //Constructors
    //     .def(py::init<>())
    //     .def(py::init<const string&, const string&>(), "path"_a, "config"_a="")
    //     .def("open", py::overload_cast<const string&, const string&>(&DOGI::open), "path"_a, "config"_a = "")
	// 	.def("populate", py::overload_cast<const string&>(&DOGI::populate), "config"_a)
    //     .def("close", &DOGI::close)
    //     ;

    py::class_<RegionSeq>(m, "RegionSeq")

    //Constructors
    .def(py::init<>())
    .def(py::init<string, string, Region>(), "name"_a, "seq"_a,
         "loc"_a = Region())
    .def(py::init<string>(), "name"_a)

    //Operators
    .def(py::self == py::self)
    .def(py::self < py::self)

    .def("setName", &RegionSeq::setName, "name"_a)
    .def("setSeq", &RegionSeq::setSeq, "seq"_a, "loc"_a=Region())
    .def("setLoc", &RegionSeq::setLoc, "loc"_a)
    .def("setChrom", &RegionSeq::setChrom, "chrom"_a)
    .def("setRange", py::overload_cast<int, int>(
        &RegionSeq::setRange), "first"_a, "last"_a)
    .def("setStrand", py::overload_cast<string>(
        &RegionSeq::setStrand), "strand"_a = "")
    .def("__str__", [](const RegionSeq& a){return a.str();})
    .def("__len__", [](const RegionSeq& a){return a.size();})


    .def("getName", &RegionSeq::getName)
    .def("getSeq", &RegionSeq::getSeq)
    .def("getLoc", &RegionSeq::getLoc)
    .def("getChrom", &RegionSeq::getName)
    .def("getFirst", &RegionSeq::getFirst)
    .def("getLast", &RegionSeq::getLast)
    .def("getLength", &RegionSeq::getLast)
    .def("isEmpty", &RegionSeq::isEmpty)

    .def("at", py::overload_cast<size_t>(&RegionSeq::at, py::const_), "pos"_a)
    .def("at", py::overload_cast<const Region&>(&RegionSeq::at, py::const_),
         "pos"_a)

    .def("getSeqSlice",
         py::overload_cast<size_t, size_t>(&RegionSeq::getSeqSlice, py::const_),
         "first"_a, "length"_a)
    .def("getSeqSlice",
         py::overload_cast<const Region&>(&RegionSeq::getSeqSlice, py::const_),
         "loc"_a)

    .def("getSlice", &RegionSeq::getSlice, "loc"_a)

    .def("toFASTA", &RegionSeq::toFASTA, "line"_a = 60, "chunk"_a = 0,
         "loc"_a = false)

    .def("countGC", &RegionSeq::countGC)
    .def("calcGCRatio", &RegionSeq::calcGCRatio)
    ;

    m.def("readFASTAFile", py::overload_cast<const string&, bool>(&FASTAReader::readFASTAFile), "file_name"_a,
          "upper"_a = false);
    // m.def("readFASTASeq", py::overload_cast<ifstream&, string&, string&, string&, bool>(&FASTAReader::readFASTASeq), "input"_a,
    //       "old_name"_a, "name"_a, "seq"_a, "upper"_a = false);

    py::class_<FASTAReader>(m, "FASTAReader")

    //Constructors
    .def(py::init<string>(), "file_name"_a)

    .def("readFASTAFile", py::overload_cast<bool>(&FASTAReader::readFASTAFile),
          "upper"_a = false)
    .def("readFASTASeq", py::overload_cast<bool>(&FASTAReader::readFASTASeq),
         "upper"_a = false);
    ;

}
