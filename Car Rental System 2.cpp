#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <ctime>
#include <conio.h> // For _getch()

using namespace std;

struct CAR
{
	int carId;
	string brand;
	string model;
	float rentperday;
	bool isavailable;
};

struct rental
{
	int carId;
	string customername;
	string phone;
	string email;
	int days;
	bool isreturned;
	string rentDate;
};


string getCurrentDate()
{
	time_t t = time(0);
	tm now;
	localtime_s(&now, &t);

	char buf[11];
	strftime(buf, sizeof(buf), "%Y-%m-%d", &now);
	return string(buf);
}


int calculateDaysLate(string rentDate, int expectedDays)
{
	tm rent = {};
	istringstream ss(rentDate);
	char dash1, dash2;
	ss >> rent.tm_year >> dash1 >> rent.tm_mon >> dash2 >> rent.tm_mday;

	if (ss.fail() || dash1 != '-' || dash2 != '-') {
		cerr << "Invalid date format!" << endl;
		return 0;
	}

	rent.tm_year -= 1900;
	rent.tm_mon -= 1;

	time_t rentTime = mktime(&rent);
	time_t currentTime = time(0);
	int daysPassed = (currentTime - rentTime) / (60 * 60 * 24);
	return max(0, daysPassed - expectedDays);
}


bool carIdExists(int id)
{
	ifstream infile("car.txt");
	if (!infile) return false;
	string line, temp;
	while (getline(infile, line))
	{
		stringstream ss(line);
		getline(ss, temp, ',');
		if (stoi(temp) == id)
			return true;
	}
	return false;
}


void addnewcar()
{
	ofstream outfile("car.txt", ios::app);
	if (!outfile) return;
	CAR car;
	cout << "Enter CAR ID: ";
	cin >> car.carId;
	if (carIdExists(car.carId))
	{
		cout << "Car ID already exists!" << endl;
		return;
	}
	cin.ignore();
	cout << "Enter brand name: ";
	getline(cin, car.brand);
	cout << "Enter model of car: ";
	getline(cin, car.model);
	cout << "Enter rent per day amount: ";
	cin >> car.rentperday;
	car.isavailable = true;
	outfile << car.carId << "," << car.brand << "," << car.model << "," << car.rentperday << "," << car.isavailable << endl;
	cout << "Car Registered Successfully." << endl;
}


void listofcars()
{
	ifstream infile("car.txt");
	if (!infile) return;
	CAR car;
	string line, temp;
	while (getline(infile, line))
	{
		stringstream ss(line);
		getline(ss, temp, ',');
		car.carId = stoi(temp);
		getline(ss, car.brand, ',');
		getline(ss, car.model, ',');
		getline(ss, temp, ',');
		car.rentperday = stof(temp);
		getline(ss, temp);
		car.isavailable = (temp == "1" || temp == "true");
		if (car.isavailable) {
			cout << "ID: " << car.carId << " | " << car.brand << " " << car.model << " | $" << car.rentperday << " per day" << endl;
		}
	}
}


bool iscaravailable(int id)
{
	ifstream infile("car.txt");
	if (!infile) return false;
	CAR car;
	string line, temp;
	while (getline(infile, line))
	{
		stringstream ss(line);
		getline(ss, temp, ',');
		car.carId = stoi(temp);
		getline(ss, car.brand, ',');
		getline(ss, car.model, ',');
		getline(ss, temp, ',');
		car.rentperday = stof(temp);
		getline(ss, temp);
		car.isavailable = (temp == "1" || temp == "true");
		if (car.carId == id && car.isavailable) return true;
	}
	return false;
}


void updateCarAvailability(int id, bool available)
{
	ifstream infile("car.txt");
	ofstream tempfile("temp.txt");
	if (!infile || !tempfile) return;
	CAR car;
	string line, temp;
	while (getline(infile, line))
	{
		stringstream ss(line);
		getline(ss, temp, ',');
		car.carId = stoi(temp);
		getline(ss, car.brand, ',');
		getline(ss, car.model, ',');
		getline(ss, temp, ',');
		car.rentperday = stof(temp);
		getline(ss, temp);
		car.isavailable = (temp == "1" || temp == "true");
		if (car.carId == id) car.isavailable = available;
		tempfile << car.carId << "," << car.brand << "," << car.model << "," << car.rentperday << "," << car.isavailable << endl;
	}
	infile.close();
	tempfile.close();
	remove("car.txt");
	rename("temp.txt", "car.txt");
}


void rentcar()
{
	ofstream outfile("rent.txt", ios::app);
	if (!outfile) return;
	rental R;
	cout << "Enter Car ID: ";
	cin >> R.carId;
	if (!iscaravailable(R.carId))
	{
		cout << "Car not available." << endl;
		return;
	}
	cin.ignore();
	cout << "Enter name: ";
	getline(cin, R.customername);
	cout << "Phone: ";
	getline(cin, R.phone);
	cout << "Email: ";
	getline(cin, R.email);
	do {
		cout << "Rent for how many days (Max 7): ";
		cin >> R.days;
		if (R.days > 7)
			cout << "Can't rent for more than 7 days!\n";
	} while (R.days > 7);

	float rate = 0.0;
	ifstream infile("car.txt");
	string line, temp;
	while (getline(infile, line))
	{
		stringstream ss(line);
		getline(ss, temp, ',');
		int tempId = stoi(temp);
		if (tempId == R.carId)
		{
			getline(ss, temp, ',');
			getline(ss, temp, ',');
			getline(ss, temp, ',');
			rate = stof(temp);
			break;
		}
	}
	infile.close();

	float total = rate * R.days;
	cout << "Total Rent: $" << total << endl;
	R.isreturned = false;
	R.rentDate = getCurrentDate();
	outfile << R.carId << "," << R.customername << "," << R.phone << "," << R.email << "," << R.days << "," << R.isreturned << "," << R.rentDate << endl;
	outfile.close();
	updateCarAvailability(R.carId, false);
	cout << "Car Rented.\n";
}


void returncar()
{
	int carId;
	cout << "Enter Car ID to return: ";
	cin >> carId;
	ifstream infile("rent.txt");
	ofstream tempfile("temp.txt");
	if (!infile || !tempfile) return;

	rental R;
	string line, temp;
	bool found = false;
	while (getline(infile, line))
	{
		stringstream ss(line);
		getline(ss, temp, ','); R.carId = stoi(temp);
		getline(ss, R.customername, ',');
		getline(ss, R.phone, ',');
		getline(ss, R.email, ',');
		getline(ss, temp, ','); R.days = stoi(temp);
		getline(ss, temp, ','); R.isreturned = (temp == "1" || temp == "true");
		getline(ss, R.rentDate);

		if (R.carId == carId && !R.isreturned)
		{
			int lateDays = calculateDaysLate(R.rentDate, R.days);
			float lateFee = lateDays * 50;
			if (lateDays > 0)
				cout << "Late by " << lateDays << " days. Late Fee = $" << lateFee << endl;
			R.isreturned = true;
			found = true;
			updateCarAvailability(R.carId, true);
		}
		tempfile << R.carId << "," << R.customername << "," << R.phone << "," << R.email << "," << R.days << "," << R.isreturned << "," << R.rentDate << endl;
	}

	infile.close();
	tempfile.close();
	remove("rent.txt");
	rename("temp.txt", "rent.txt");
	if (found)
		cout << "Car returned successfully.\n";
	else
		cout << "Rental not found.\n";
}


void cancelrent()
{
	int carId;
	cout << "Enter Car ID to cancel rent: ";
	cin >> carId;
	ifstream infile("rent.txt");
	ofstream tempfile("temp.txt");
	if (!infile || !tempfile) return;

	rental R;
	string line, temp;
	bool found = false;
	while (getline(infile, line))
	{
		stringstream ss(line);
		getline(ss, temp, ','); R.carId = stoi(temp);
		getline(ss, R.customername, ',');
		getline(ss, R.phone, ',');
		getline(ss, R.email, ',');
		getline(ss, temp, ','); R.days = stoi(temp);
		getline(ss, temp, ','); R.isreturned = (temp == "1" || temp == "true");
		getline(ss, R.rentDate);

		if (R.carId == carId && !R.isreturned)
		{
			found = true;
			updateCarAvailability(carId, true);
			continue;
		}
		tempfile << R.carId << "," << R.customername << "," << R.phone << "," << R.email << "," << R.days << "," << R.isreturned << "," << R.rentDate << endl;
	}

	infile.close();
	tempfile.close();
	remove("rent.txt");
	rename("temp.txt", "rent.txt");
	if (found)
		cout << "Rent cancelled.\n";
	else
		cout << "Not found.\n";
}


void listLateReturns()
{
	ifstream infile("rent.txt");
	if (!infile) {
		cout << "Could not open rent.txt\n";
		return;
	}

	string line, temp;
	bool found = false;
	float totalLateFees = 0;
	while (getline(infile, line))
	{
		rental R;
		stringstream ss(line);
		getline(ss, temp, ','); R.carId = stoi(temp);
		getline(ss, R.customername, ',');
		getline(ss, R.phone, ',');
		getline(ss, R.email, ',');
		getline(ss, temp, ','); R.days = stoi(temp);
		getline(ss, temp, ','); R.isreturned = (temp == "1" || temp == "true");
		getline(ss, R.rentDate);

		if (!R.isreturned)
		{
			int lateDays = calculateDaysLate(R.rentDate, R.days);
			if (lateDays > 0)
			{
				found = true;
				float lateFee = lateDays * 50;
				totalLateFees += lateFee;
				cout << "\n?? Late Return:\n";
				cout << "Car ID: " << R.carId << " | Customer: " << R.customername << "\n";
				cout << "Phone: " << R.phone << ", Email: " << R.email << "\n";
				cout << "Rented Days: " << R.days << ", Rent Date: " << R.rentDate << "\n";
				cout << "Late by: " << lateDays << " days | Late Fee: $" << lateFee << endl;
			}
		}
	}
	infile.close();
	if (!found)
		cout << "\n No late returns found.\n";
	else
		cout << "\n Total Late Fees Due: $" << totalLateFees << endl;
}


bool adminLogin() {
	string username, password;
	char ch;

	cout << "\nAdmin Login\n";
	cout << "Username: ";
	cin >> username;

	cout << "Password: ";
	ch = _getch();
	while (ch != 13) {
		if (ch == 8 && !password.empty()) {
			cout << "\b \b";
			password.pop_back();
		}
		else if (ch != 8) {
			password.push_back(ch);
			cout << '*';
		}
		ch = _getch();
	}
	cout << endl;

	string correctUsername = "admin";
	string correctPassword = "1234";

	if (username == correctUsername && password == correctPassword) {
		cout << "Login successful!\n";
		return true;
	}
	else {
		cout << "Invalid credentials!\n";
		return false;
	}
}


int main()
{
	int roleChoice;
	do {
		cout << "\nSelect Role:\n1 - Admin\n2 - User\n3 - Exit\nEnter: ";
		cin >> roleChoice;
		int choice;
		switch (roleChoice)
		{
		case 1:
			if (adminLogin()) {
				do {
					cout << "\n-- Admin Menu --\n";
					cout << "1 - Add New Car\n";
					cout << "2 - List Cars\n";
					cout << "3 - View Late Returns\n";
					cout << "4 - Back\nEnter: ";
					cin >> choice;
					if (choice == 1)
						addnewcar();
					else if (choice == 2)
						listofcars();
					else if (choice == 3)
						listLateReturns();
				} while (choice != 4);
			}
			break;
		case 2:
			do {
				cout << "\n-- User Menu --\n1 - Rent Car\n2 - Return Car\n3 - Cancel Rent\n4 - Back\nEnter: ";
				cin >> choice;
				if (choice == 1)
					rentcar();
				else if (choice == 2)
					returncar();
				else if (choice == 3)
					cancelrent();
			} while (choice != 4);
			break;
		case 3:
			cout << "Goodbye!\n";
			break;
		default:
			cout << "Invalid choice!\n";
		}
	} while (roleChoice != 3);

	return 0;
}
