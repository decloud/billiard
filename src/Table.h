#ifndef TABLE_H
#define TABLE_H

//TODO: keep track of location of pockets, size of pockets, color maybe
//TODO: allow presets by overload constructors
//TODO: might also add a method that gives the conversion factor
//NOTE: should a table have the balls? or should the balls be in billiard?
//NOTE: should the table keep track of its own xyz coordinate?

class Table
{
	public:
		Table(float length);
		Table(float length, float width);
		float length;
		float width;
};

#endif