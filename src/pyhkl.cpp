
// #include "dogitoys.h"

#include <pybind11/iostream.h>
#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
// #include <exception>

#include "hkl/region.hpp"
#include "hkl/regionseq.hpp"
// #include "dogitoys.h"

namespace py = pybind11;
using namespace pybind11::literals;
using namespace HKL;
// using namespace DOGIToys;

using std::ifstream;
using std::string;

PYBIND11_MODULE(pyHKL, m) {
  py::register_exception<RegionError>(m, "RegionError");

  py::class_<Region>(m, "Region")
      // Constructors
      .def(py::init<>())
      .def(py::init<string, int, int, string>(), "chrom"_a, "first"_a, "last"_a,
           "strand"_a = "")
      .def(py::init<string, int, string>(), "chrom"_a, "pos"_a, "strand"_a = "")
      .def(py::init<string>(), "query"_a)

      // Operators
      // .def(py::init<>())
      .def(py::self == py::self)
      .def(py::self < py::self)
      .def(py::self > py::self)
      .def(py::self <= py::self)
      .def(py::self >= py::self)

      // Setters
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
      .def("resize",
           py::overload_cast<std::pair<int, int>, bool>(&Region::resize),
           "extent"_a, "orient"_a = false)

      // Checkers
      .def("isEmpty", &Region::isEmpty)
      .def("isPos", &Region::isPos)
      .def("isRange", &Region::isRange)
      .def("isPure", &Region::isPure)

      // Getters
      .def("getChrom", py::overload_cast<>(&Region::getChrom, py::const_))
      .def("getChrom",
           py::overload_cast<const Region &>(&Region::getChrom, py::const_),
           "other"_a)

      .def("getFirst", &Region::getFirst)
      .def("getCenter", &Region::getCenter)
      .def("getLast", &Region::getLast)
      .def("getRange", &Region::getRange)

      .def("getStrand",
           [](const Region &self) {
             if (auto strand = self.getStrand(); strand)
               return string(1, strand);
             else
               return string();
           })
      .def("getStrand",
           [](const Region &self, const Region &other) {
             if (auto strand = self.getStrand(other); strand)
               return string(1, strand);
             else
               return string();
           })

      .def("getLength", &Region::getLength)

      .def("at", &Region::at, "pos"_a)
      .def("str", &Region::str)

      // Compare
      .def("sameChrom", &Region::sameChrom, "other"_a)
      .def("sharesChrom", &Region::sharesChrom, "other"_a)

      .def("sameRange", &Region::sameRange, "other"_a)
      .def("sharesRange", &Region::sharesRange, "other"_a)
      .def("sharesPos", &Region::sharesPos, "pos"_a)

      .def("sameStrand", &Region::sameStrand, "other"_a)
      .def("sharesStrand", &Region::sharesStrand, "other"_a)

      // Relationship
      .def("dist", &Region::dist, "Calculate distance between regions",
           "other"_a, "orient"_a = false)

      .def("shares", &Region::shares, "other"_a)
      .def("around", &Region::around, "other"_a)
      .def("next", &Region::next, "other"_a)
      .def("inside", &Region::inside, "other"_a)
      .def("covers", &Region::covers, "other"_a)

      .def("upstream", &Region::upstream, "other"_a, "orient"_a = true)
      .def("downstream", &Region::downstream, "other"_a, "orient"_a = true)

      // Generating Regions
      .def("getShared", &Region::getShared, "other"_a)
      .def("getUnion", &Region::getUnion, "other"_a)
      .def("getDiff", &Region::getDiff, "other"_a)
      .def("getGap", &Region::getGap, "other"_a)
      .def("getSlice", &Region::getSlice, "pos"_a, "length"_a = 0)
      .def("getSlices", &Region::getSlices, "length"_a)

      // Share Rate
      .def("getSharedLength", &Region::getSharedLength, "other"_a)
      .def("getCoveredRate", &Region::getCoveredRate, "other"_a)
      .def("getCapturedRate", &Region::getCapturedRate, "other"_a)

      // Relative positions
      .def("getRelPos",
           py::overload_cast<const Region &, bool, bool>(&Region::getRelPos,
                                                         py::const_),
           "other"_a, "orient"_a = false, "last"_a = false)
      .def("getRelPos",
           py::overload_cast<int, bool, bool>(&Region::getRelPos, py::const_),
           "pos"_a, "orient"_a = false, "last"_a = false)
      .def("getRelPosLast",
           py::overload_cast<const Region &, bool>(&Region::getRelPosLast,
                                                   py::const_),
           "other"_a, "orient"_a = false)
      .def("getRelPosLast",
           py::overload_cast<int, bool>(&Region::getRelPosLast, py::const_),
           "pos"_a, "orient"_a = false)
      .def("getRelPosRatio",
           py::overload_cast<const Region &, bool, bool>(
               &Region::getRelPosRatio, py::const_),
           "other"_a, "orient"_a = false, "last"_a = false)
      .def("getPosRelRatio",
           py::overload_cast<int, bool, bool>(&Region::getRelPosRatio,
                                              py::const_),
           "pos"_a, "orient"_a = false, "last"_a = false)
      .def("getRelPosLastRatio",
           py::overload_cast<const Region &, bool>(&Region::getRelPosLastRatio,
                                                   py::const_),
           "other"_a, "orient"_a = false)
      .def(
          "getPosRelRatio",
          py::overload_cast<int, bool>(&Region::getRelPosLastRatio, py::const_),
          "pos"_a, "orient"_a = false)

      // Python specific
      .def("__repr__",
           [](const Region &a) { return "<HKL.Region in " + a.str() + ">"; })
      .def("__str__", [](const Region &a) { return a.str(); })
      .def("__hash__", [](const Region &a) { return std::hash<Region>{}(a); })
      .def("__len__", [](const Region &a) { return a.getLength(); });

  py::class_<RegionSeq>(m, "RegionSeq")

      // Constructors
      .def(py::init<>())
      .def(py::init<string, string, Region>(), "name"_a, "seq"_a,
           "loc"_a = Region())
      .def(py::init<string>(), "name"_a)

      // Operators
      .def(py::self == py::self)
      .def(py::self < py::self)

      .def("setName", &RegionSeq::setName, "name"_a)
      .def("setSeq", &RegionSeq::setSeq, "seq"_a, "loc"_a = Region())
      .def("setLoc", &RegionSeq::setLoc, "loc"_a)
      .def("setChrom", &RegionSeq::setChrom, "chrom"_a)
      .def("setRange", py::overload_cast<int, int>(&RegionSeq::setRange),
           "first"_a, "last"_a)
      .def("setStrand", py::overload_cast<string>(&RegionSeq::setStrand),
           "strand"_a = "")
      .def("__str__", [](const RegionSeq &a) { return a.str(); })
      .def("__len__", [](const RegionSeq &a) { return a.size(); })

      .def("getName", &RegionSeq::getName)
      .def("getSeq", py::overload_cast<>(&RegionSeq::getSeq, py::const_))
      .def("getLoc", &RegionSeq::getLoc)
      .def("getChrom", &RegionSeq::getName)
      .def("getFirst", &RegionSeq::getFirst)
      .def("getLast", &RegionSeq::getLast)
      .def("getLength", &RegionSeq::getLast)
      .def("isEmpty", &RegionSeq::isEmpty)

      //      .def("at", py::overload_cast<int>(&RegionSeq::at, py::const_),
      //      "pos"_a)
      .def("at", &RegionSeq::at, "pos"_a)

      .def("getSeq",
           py::overload_cast<int, size_t>(&RegionSeq::getSeq, py::const_),
           "first"_a, "length"_a)
      .def("getSeq",
           py::overload_cast<const Region &>(&RegionSeq::getSeq, py::const_),
           "loc"_a)

      .def("getSlice", &RegionSeq::getSlice, "loc"_a)

      .def("toFASTA", &RegionSeq::toFASTA, "line"_a = 60, "chunk"_a = 0,
           "loc"_a = false)

      .def("countGC", &RegionSeq::countGC)
      .def("calcGCRatio", &RegionSeq::calcGCRatio);

  py::class_<FASTAReader>(m, "FASTAReader")
      // Constructors
      .def(py::init<string>(), "file_name"_a)

      .def("open", &FASTAReader::open, "file_name"_a)
      .def("close", &FASTAReader::close)
      .def("good", &FASTAReader::good)
      .def("getSeq", &FASTAReader::getSeq)
      .def("readFile", &FASTAReader::readFile, "upper"_a = false)
      .def("readSeq", &FASTAReader::readSeq, "upper"_a = false);
}
