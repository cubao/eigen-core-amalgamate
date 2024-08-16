#define DBG_MACRO_NO_WARNING
#include "dbg.h"

#include <vector>
#include <iostream>

#include <Eigen/Core>
#include <Eigen/SparseCore>

using RowVectors = Eigen::Matrix<double, Eigen::Dynamic, 3, Eigen::RowMajor>;
using RowVectorsNx3 = RowVectors;
using RowVectorsNx2 = Eigen::Matrix<double, Eigen::Dynamic, 2, Eigen::RowMajor>;

void update(Eigen::Ref<RowVectorsNx2> xys) {
	xys.col(0) *= 10;
	xys.col(1) *= 20;
}

int main(int argc, char **argv) {
	dbg(EIGEN_WORLD_VERSION);
	dbg(EIGEN_MAJOR_VERSION);
	dbg(EIGEN_MINOR_VERSION);

	Eigen::Vector3d xyz(1, 2, 3);
	dbg(xyz.transpose());

	Eigen::VectorXd coeffs = (Eigen::VectorXd(5) << 4, 5, 6, 7, 8).finished();
	dbg(coeffs.transpose());

	auto abcd = std::vector<double>{1, 3, 5, 7};
	dbg(abcd);
	Eigen::Map<Eigen::Vector4d>(&abcd[0], 4) *= 10;
	dbg(abcd);

	auto mat = std::vector<double>{0, 1, 2, 3, 4, 5};
	Eigen::Map<RowVectorsNx3> M(&mat[0], 2, 3);
	std::cout << M << std::endl;
	update(M.leftCols(2));
	std::cout << M << std::endl;
	// dbg(M); // https://github.com/sharkdp/dbg-macro/issues/131

	// https://eigen.tuxfamily.org/dox/group__SparseCore__Module.html

	return 0;
}
