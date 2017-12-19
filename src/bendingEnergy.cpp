#include "../include/bendingEnergy.h"

vector<int> find_chain(vector<Point> &contourVec, Point &start);
pair<int, Point> find_first_neighbour(vector<Point> &vec, Point &start, vector<Point> &done);
pair<int, Point> find_eight_neighbours(vector<Point> &vec, Point &p, int offset, vector<Point> &done);
bool vec_contains(vector<Point> &vec, Point &p);

double bendingEnergy(vector<Point>& contourVec)
{
	// cout << "In function be	ndingEnergy" << endl;
	// cout << "Size of vec is " << contourVec.size() << endl;

	// find start pixel, so x is closest to 0 and y is also closest to 0
	// note, multiple x'es having the same value can occur.
	Point *start;
	start = &contourVec.at(0);
	for (size_t i = 0; i < contourVec.size(); i++) {
		Point *p = &contourVec.at(i);
		if ((p->x <= start->x) && (p->y <= start->y)) {
			start = p;
		}
	}

	// cout << "Most top left is " << *start << endl;

	// Assume that a 45 degree bend requires 1 energy, assume that a 90 degree bend requires 2.
	double energy = 0.0;

	vector<int> chain = find_chain(contourVec, *start);
	// cout << "Found chain is " << chain.size() << endl;
	if (chain.size() > 1) {
		int last = chain[0];
		for (size_t i = 1; i < chain.size(); i++) {
			// cout << chain[i];
			int current = chain[i];
			// diff of 1 adds 1, diff of 2 adds 2, diff of 3 adds 3
			energy += abs((current - last));
			last = current;
		}
		// cout << endl;
	}

	return energy;
}

vector<int> find_chain(vector<Point> &contourVec, Point &start) {
	vector<int> chain;
	Point *first = &start;
	Point *current = &start;
	vector<Point> found;
	found.push_back(start);
	bool running = true;
	while (running) {
		// cout << "Finding for " << *current << endl;
		pair<int, Point> p = find_first_neighbour(contourVec, *current, found);
		// cout << "We have found " << p.second << " with id " << p.first << endl;
		// cout << "--------------" << endl;
		found.push_back(p.second);
		if (p.first == -1) {
			return chain;
		}
		// find next
		current = &p.second;
		chain.push_back(p.first);
	}

	return chain;
}

pair<int, Point> find_first_neighbour(vector<Point> &vec, Point &start, vector<Point> &done) {
	for (size_t i = 1; i <= 10; i++) {
		pair<int, Point> p = find_eight_neighbours(vec, start, i, done);
		if (p.second != start) {
			return p;
		}
	}
	return make_pair(-1, start);
}

pair<int, Point> find_eight_neighbours(vector<Point> &vec, Point &p, int offset, vector<Point> &done) {
	// P is our starting point.
	// cout << "Starting from "  << p << endl;
	for (size_t i = 0; i < 8; i++) {
		Point to_check;
		switch (i) {
		case 0:
			// check right
			to_check = Point(p.x + offset, p.y);
			break;
		case 1:
			// check right top
			to_check = Point(p.x + offset, p.y + offset);
			break;
		case 2:
			// check top
			to_check = Point(p.x, p.y + offset);
			break;
		case 3:
			// check left top
			to_check = Point(p.x - offset, p.y + offset);
			break;
		case 4:
			// check left
			to_check = Point(p.x - offset, p.y);
			break;
		case 5:
			// check left bot
			to_check = Point(p.x - offset, p.y - offset);
			break;
		case 6:
			// check bot
			to_check = Point(p.x, p.y - offset);
			break;
		case 7:
			// check right bot
			to_check = Point(p.x + offset, p.y - offset);
			break;
		default:
			cerr << "This should never happen in find_eight_neighbours" << endl;
			break;
		}
		// cout << "Checking " << to_check << endl;
		if (vec_contains(vec, to_check) && !vec_contains(done, to_check)) {
			// cout << "This point is in vec" << endl;
			return make_pair(i, to_check);
		}
	}
	return make_pair(-1, p);
}

// return true if vec contains point p
bool vec_contains(vector<Point> &vec, Point &p) {
	for (size_t i = 0; i < vec.size(); i++) {
		Point *p2 = &vec.at(i);
		if (p.x == p2->x && p.y == p2->y) {
			return true;
		}
	}
	return false;
}
