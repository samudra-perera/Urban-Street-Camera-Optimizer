#!/usr/bin/env python3
import sys, re


class Point(object):
    """
    A class for a point on the graph taken from Dr. Gurfinkel's example py code
    """

    def __init__(self, x, y):
        self.x = float(x)
        self.y = float(y)

    def __str__(self):
        return "(" + str(self.x) + "," + str(self.y) + ")"


class Line(object):
    """
    A class for a line taken from Dr. Gurfinkel's example py code
    """

    def __init__(self, src, dst):
        # Src = source point. and dst = destination point
        self.src = src
        self.dst = dst

    def __str__(self):
        return str(self.src) + "-->" + str(self.dst)


def intersect(l1, l2):
    # Needs to handle the case of no parallel lines
    # Case of no intersection
    x1, y1 = l1.src.x, l1.src.y
    x2, y2 = l1.dst.x, l1.dst.y
    x3, y3 = l2.src.x, l2.src.y
    x4, y4 = l2.dst.x, l2.dst.y

    xden = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4)
    yden = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4)
    xnum = (x1 * y2 - y1 * x2) * (x3 - x4) - (x1 - x2) * (x3 * y4 - y3 * x4)
    ynum = (x1 * y2 - y1 * x2) * (y3 - y4) - (y1 - y2) * (x3 * y4 - y3 * x4)
    u_num = (x1 - x2) * (y1 - y3) - (y1 - y2) * (x1 - x3)
    t_num = (x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)

    if xden == 0 or yden == 0:
        return None
    # This was taken from Wikipedia
    # https://en.wikipedia.org/wiki/Line%E2%80%93line_intersection
    t = t_num / xden
    u = -(u_num / xden)

    if t < 0 or t > 1 or u < 0 or u > 1:
        return None
    else:
        xcoor = xnum / xden
        ycoor = ynum / yden
        return Point(xcoor, ycoor)


class StreetMap:
    def __init__(self):
        self.street_db = {}
        self.line_db = {}
        self.vertex = {}
        self.edges = []
        self.dict_of_intersections = {}

    def add_street(self, street_input):
        """
        Takes in a street with the format 'add "{street_name}" (x1, y1) (x2, y2) (x_n, y_n)'

        Stores the added street into a dictionary and adds street points in a list of tuples
        """
        # Use a regular expression to match the input format in description
        add_pattern = r'^\s*add\s+"([A-Za-z ]*[A-Za-z][A-Za-z ]*)"((?:\s*\(\s*-?\d+\s*,\s*-?\d+\s*\)){2,})$'
        match = re.match(add_pattern, street_input)

        # If the format is valid add the street to the dictionary
        # Else return an error stating the format is wrong
        if match:
            # Extracting groups from the regex match
            street_name = match.group(
                1
            ).lower()  # Make street_name lowered to handle case insensitivity
            coordinates = match.group(2)

            # Check if street_name is unique
            if street_name in self.street_db:
                sys.stderr.write(
                    f"Error: {street_name} already exists in the database\n"
                )

            # Setting the street key to the array
            self.street_db[street_name] = self._street_coordinate_array(coordinates)

        else:
            sys.stderr.write("Error: does not follow the correct input\n")

    def remove_street(self, street_input):
        """
        This function is called when an input is in the form 'rm "{street_input}"'

        The purpose of this function is to delete a street_name: coords key-value pair from the database
        """
        # Regex pattern for input validation
        remove_pattern = r'^\s*rm\s+"([A-Za-z ]*[A-Za-z][A-Za-z ]*)"\s*$'
        match = re.match(remove_pattern, street_input)

        # If valid, check if a street name exists in the database
        if match:
            street_name = match.group(1).lower()
            if street_name not in self.street_db:
                sys.stderr.write(
                    f"Error: The street {street_name} you are trying to remove does not exist in the database\n"
                )

            del self.street_db[street_name]

        else:
            sys.stderr.write("Error: Input Error\n")

    def _street_coordinate_array(self, coordinates):
        """
        Takes in a string of coordinates and converts it into a list of Point objects
        """
        # Create the list of strings from the coordinates using regex to only find all digits
        street_coordinates = re.findall(r"\-?\d+,\s?-?\d+", coordinates)
        street_coordinates_list = []

        # Process of cleaning the string into coordinate tuples then pushing into an array
        for coord in street_coordinates:
            x, y = coord.split(",")
            coord_point = Point(float(x), float(y))
            street_coordinates_list.append(coord_point)

        return street_coordinates_list

    def modify_street(self, street_input):
        """
        This function is called when the user inputs a line in the form 'mod "{street_input}" (2,1) (2,2) (x,y)'
        """
        # Regex pattern for input validation
        mod_pattern = r'^\s*mod\s+"([A-Za-z ]*[A-Za-z][A-Za-z ]*)"((?:\s*\(\s*-?\d+\s*,\s*-?\d+\s*\)){2,})\s*$'

        match = re.match(mod_pattern, street_input)

        if match:
            street_name = match.group(1).lower()
            coordinates = match.group(2)

            if street_name not in self.street_db:
                sys.stderr.write(
                    "Error: You are trying to modify a street that does not exist\n"
                )

            self.street_db[street_name] = self._street_coordinate_array(coordinates)
        else:
            sys.stderr.write("Error: Invalid input format for modification\n")

    def _make_line_list(self, street_db):
        """
        Takes in a street_db = {'street name': [Point(objects), ...]}

        returns list_db = {'street_name': [(Line(objects))]}
        """
        line_db = {}  # Initialize line_db as a local dictionary

        for street in street_db:
            temp = []
            for i in range(len(street_db[street]) - 1):
                src_point = street_db[street][i]
                dst_point = street_db[street][i + 1]
                line = Line(src_point, dst_point)
                temp.append(line)

            line_db[street] = temp

        return line_db

    def _make_graph(self):
        """
        Takes in a line_db = {'street name': [line(object), ...]}
        returns a dictionary of list of Vertexes = [(px, py): counter, ...]
        returns a list of edges as tuples Edge = [(vertex1, vertex2) ...]
        """
        V = {}  # Initialize V as a local dictionary
        E = []  # Initialize E as a local list
        line_list = []
        line_vertex_dict = {}

        line_db = self._make_line_list(self.street_db)  # Get the line_db

        street_list = list(line_db.keys())
        # Counter for the dictionary values
        counter = 1
        # Iterate through the streets
        for i in range(len(street_list)):
            # Set current street to the first street
            current_street = street_list[i]
            # Iterate over all other streets (not including the current street)
            for j in range(len(street_list)):
                other_street = street_list[j]
                # AS long as the current street does not equal itself iterate line by line
                if current_street != other_street:
                    # Line1 in the current street
                    for line1 in line_db[current_street]:
                        # Line2 in the other street
                        for line2 in line_db[other_street]:
                            # Check Intersection
                            point = intersect(line1, line2)
                            # If no intersection, i.e., not vertex or edge, pass
                            if point is None:
                                pass
                            else:
                                # Push the lines into the list
                                if line1 not in line_list:
                                    line_list.append(line1)

                                if line2 not in line_list:
                                    line_list.append(line2)
                                # Check if intersection already exists, if not append to list
                                # Deconstruct the point
                                [x, y] = point.x, point.y
                                intersect_tuple = tuple([x, y])

                                # The points to be used in the vertex creation
                                point1 = tuple([line1.src.x, line1.src.y])
                                point2 = tuple([line1.dst.x, line1.dst.y])
                                point3 = tuple([line2.src.x, line2.src.y])
                                point4 = tuple([line2.dst.x, line2.dst.y])

                                # Create a list of all the vertexes
                                temp_list = [
                                    point1,
                                    point2,
                                    point3,
                                    point4,
                                    intersect_tuple,
                                ]

                                # Check if any of the vertexes currently exist, if they do not add to the dictionary and increment
                                for point in temp_list:
                                    if point not in V:
                                        V[point] = counter
                                        counter += 1
                                # Creating a dictionary of lines in the shape
                                # line = {'line': [point1, point2, intersection]}
                                for line in line_list:
                                    # Keys are the lines 1 and 2 from above but as a pair of Vertex Tuples
                                    key1 = (V[point1], V[point2])
                                    key2 = (V[point3], V[point4])

                                    # If keys do not exist in the dictionary add in the following shape
                                    if key1 not in line_vertex_dict:
                                        line_vertex_dict[key1] = [
                                            point1,
                                            point2,
                                            intersect_tuple,
                                        ]
                                    elif key2 not in line_vertex_dict:
                                        line_vertex_dict[key2] = [
                                            point3,
                                            point4,
                                            intersect_tuple,
                                        ]
                                    elif key1 in line_vertex_dict:
                                        if (
                                            intersect_tuple
                                            not in line_vertex_dict[key1]
                                        ):
                                            line_vertex_dict[key1].append(
                                                intersect_tuple
                                            )
                                    elif key2 in line_vertex_dict:
                                        if (
                                            intersect_tuple
                                            not in line_vertex_dict[key2]
                                        ):
                                            line_vertex_dict[key2].append(
                                                intersect_tuple
                                            )

        # Sort the list of tuples within each value based on the x-coordinate
        sorted_line_vertex_dict = {
            key: sorted(value, key=lambda point: point[0])
            for key, value in line_vertex_dict.items()
        }
        # Now make the edges
        # If the x coordinate is the same, use the y coordinate to sort (in the case of vertical lines)
        for line in sorted_line_vertex_dict:
            sorted_vertices = sorted(
                sorted_line_vertex_dict[line], key=lambda vertex: (vertex[0], vertex[1])
            )

            for i in range(len(sorted_vertices) - 1):
                # Check if the vertices are the same
                if sorted_vertices[i] != sorted_vertices[i + 1]:
                    edge = (V[sorted_vertices[i]], V[sorted_vertices[i + 1]])
                    E.append(edge)

        self.vertex = V  # Update the instance attribute
        self.edges = E  # Update the instance attribute
        return V, E


def main():
    street_map = StreetMap()

    while True:
        try:
            street_input = input().strip()

            if not street_input:
                continue

            # Parse the command and arguments
            tokens = re.split(r'["()]|\s+', street_input)
            command = tokens[0]

            if command == "add":
                street_map.add_street(street_input)
            elif command == "rm":
                street_map.remove_street(street_input)
            elif command == "mod":
                street_map.modify_street(street_input)
            elif command == "gg":
                V, E = street_map._make_graph()

                # Print Vertex
                print("V = {")
                for coordinates, vertex_id in V.items():
                    coordinates1 = "{0:.2f}".format(coordinates[0])
                    coordinates2 = "{0:.2f}".format(coordinates[1])
                    print(f"{vertex_id}: ({coordinates1},{coordinates2})")
                print("}")

                # Print edges
                print("E = {")
                for i, edge in enumerate(E):
                    print(
                        f"<{edge[0]},{edge[1]}>", end=",\n" if i < len(E) - 1 else "\n"
                    )
                print("}")
            else:
                sys.stderr.write("Error: Please enter a valid command\n")

        except EOFError:
            break

    sys.exit(0)


if __name__ == "__main__":
    main()
