
import csv
import os.path
from datetime import datetime

def savetocsv(id,x,y,z,yaw,frame, final ):
    # Define the CSV file path
    csv_file = 'aruco_results.csv'

    # Check if the file already exists
    file_exists = os.path.isfile(csv_file)
    # Write the ArUco marker results to the CSV file
    with open(csv_file, 'a', newline='') as file:
        writer = csv.writer(file)

        if file_exists:
            # Write a new header row with starting time and date
            writer.writerow([])  # Empty row for clarity
            # writer.writerow(['New Data Starting'])
            # writer.writerow(['Starting Time:', datetime.now().strftime('%Y-%m-%d %H:%M:%S')])
            # writer.writerow([])  # Empty row for clarity
            # Write the header row
            if final ==True: 
                header = ['ID', 'X', 'Y', 'Z', 'Yaw', 'FINAL']
                # print('final')
            if final == False:
                header = ['ID', 'X', 'Y', 'Z', 'Yaw', 'ORIG']
                # print('original')

            writer.writerow(header)
            row = id,x,y,z,yaw,['Starting Time:', datetime.now().strftime('%H:%M:%S')]
            writer.writerow(row)
            
if __name__ == "__main__":
    savetocsv(1,2,3,4,5,6,False)