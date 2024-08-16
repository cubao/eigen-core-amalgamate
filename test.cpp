#define DBG_MACRO_NO_WARNING
#include "dbg.h"

#include <Eigen/Core>
#include <vector>

int main(int argc, char **argv) {
	Eigen::Vector3d xyz(1, 2, 3);
	dbg(xyz.transpose());

	Eigen::VectorXd coeffs = (Eigen::VectorXd(5) << 4, 5, 6, 7, 8).finished();
	dbg(coeffs.transpose());

	auto abcd = std::vector<double>{1, 3, 5, 7};
	dbg(abcd);
	Eigen::Map<Eigen::Vector4d>(&abcd[0], 4) *= 10;
	dbg(abcd);

	return 0;
}
