//
// File:        ix_testshell.cc
// Description: Test IX component
// Authors:     Jan Jannink
//              Dallan Quass (quass@cs.stanford.edu)
//
// This test shell contains a number of functions that will be useful in
// testing your IX component code.  In addition, a couple of sample
// tests are provided.  The tests are by no means comprehensive, you are
// expected to devise your own tests to test your code.
//
// 1997:  Tester has been modified to reflect the change in the 1997
// interface.
// 2000:  Tester has been modified to reflect the change in the 2000
// interface.

#include <cstdio>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <ctime>

#include "redbase.h"
#include "pf.h"
#include "rm.h"
#include "ix.h"
#include "pf_buffermgr.h"

using namespace std;

//
// Defines
//
#define FILENAME     "testrel"        // test file name
#define BADFILE      "/abc/def/xyz"   // bad file name
#define STRLEN       39               // length of strings to index
#define FEW_ENTRIES  20
#define MANY_ENTRIES 57971
//#define MANY_ENTRIES 2000
#define NENTRIES     200000           // Size of values array
#define PROG_UNIT    200              // how frequently to give progress
// reports when adding lots of entries

//
// Values array we will be using for our tests
//
int values[NENTRIES];

//
// Global component manager variables
//
PF_Manager pfm;
RM_Manager rmm(pfm);
IX_Manager ixm(pfm);

//
// Function declarations
//
RC Test1(void);
RC Test2(void);
RC Test3(void);
RC Test4(void);
RC Test5(void);
RC Test6(void);
RC Test7(void);

void PrintError(RC rc);
void LsFiles(const char *fileName);
void ran(int n);
RC InsertIntEntries(IX_IndexHandle &ih, int nEntries);
RC InsertFloatEntries(IX_IndexHandle &ih, int nEntries);
RC InsertStringEntries(IX_IndexHandle &ih, int nEntries);
RC AddRecs(RM_FileHandle &fh, int nRecs);
RC DeleteIntEntries(IX_IndexHandle &ih, int nEntries);
RC DeleteFloatEntries(IX_IndexHandle &ih, int nEntries);
RC DeleteStringEntries(IX_IndexHandle &ih, int nEntries);
RC VerifyIntIndex(IX_IndexHandle &ih, int nStart, int nEntries, int bExists);
RC PrintIndex(IX_IndexHandle &ih);

//
// Array of pointers to the test functions
//
#define NUM_TESTS       7               // number of tests
int (*tests[])() = // RC doesn't work on some compilers
{
   Test1,
   Test2,
   Test3,
   Test4,
   Test5,
   Test6,
   Test7
};

//
// main
//

int main(int argc, char *argv[]) {
   RC rc;
   char *progName = argv[0]; // since we will be changing argv
   int testNum;

   // Write out initial starting message
   printf("Starting IX component test.\n\n");

   // Init randomize function
   srand((unsigned) time(NULL));

   // Delete files from last time (if found)
   // Don't check the return codes, since we expect to get an error
   // if the files are not found.
   rmm.DestroyFile(FILENAME);
   ixm.DestroyIndex(FILENAME, 0);
   ixm.DestroyIndex(FILENAME, 1);
   ixm.DestroyIndex(FILENAME, 2);
   ixm.DestroyIndex(FILENAME, 3);
   ixm.DestroyIndex(FILENAME, 4);
   ixm.DestroyIndex(FILENAME, 5);
   ixm.DestroyIndex(FILENAME, 6);

   // If no argument given, do all tests
   if (argc == 1) {
      for (testNum = 0; testNum < NUM_TESTS; testNum++)
         if ((rc = (tests[testNum])())) {
            // Print the error and exit
            PrintError(rc);
            return (1);
         }
   } else {
      // Otherwise, perform specific tests
      while (*++argv != NULL) {

         // Make sure it's a number
         if (sscanf(*argv, "%d", &testNum) != 1) {
            cerr << progName << ": " << *argv << " is not a number\n";
            continue;
         }

         // Make sure it's in range
         if (testNum < 1 || testNum > NUM_TESTS) {
            cerr << "Valid test numbers are between 1 and " << NUM_TESTS << "\n";
            continue;
         }

         // Perform the test
         if ((rc = (tests[testNum - 1])())) {
            // Print the error and exit
            PrintError(rc);
            return (1);
         }
      }
   }

   // Write ending message and exit
   printf("Ending IX component test.\n\n");

   return (0);
}

//
// PrintError
//
// Desc: Print an error message by calling the proper component-specific
//       print-error function
//

void PrintError(RC rc) {
   if (abs(rc) <= END_PF_WARN)
      PF_PrintError(rc);
   else if (abs(rc) <= END_RM_WARN)
      RM_PrintError(rc);
   else if (abs(rc) <= END_IX_WARN)
      IX_PrintError(rc);
   else
      cerr << "Error code out of range: " << rc << "\n";
}

////////////////////////////////////////////////////////////////////
// The following functions may be useful in tests that you devise //
////////////////////////////////////////////////////////////////////

//
// LsFiles
//
// Desc: list the filename's directory entry
//

void LsFiles(const char *fileName) {
   char command[80];

   sprintf(command, "ls -l *%s*", fileName);
   printf("Doing \"%s\"\n", command);
   system(command);
}

//
// Create an array of random nos. between 0 and n-1, without duplicates.
// put the nos. in values[]
//

void ran(int n) {
   int i = 0, r = 0, t = 0, m = 0;

   // Initialize values array
   for (i = 0; i < NENTRIES; i++)
      values[i] = i;

   // Randomize first n entries in values array
//   for (i = 0, m = n; i < n - 1; i++) {
//      r = (int) (rand() % m--);
//      t = values[m];
//      values[m] = values[r];
//      values[r] = t;
//   }
}

//
// InsertIntEntries
//
// Desc: Add a number of integer entries to the index
//

RC InsertIntEntries(IX_IndexHandle &ih, int nEntries) {
   RC rc;
   int i;
   int value;

   printf("             Adding %d int entries\n", nEntries);
   ran(nEntries);
   for (i = 0; i < nEntries; i++) {
      value = values[i] + 1;
      RID rid(value, value * 2);
      if (i < 340) {
         if ((rc = ih.InsertEntry((void *) &value, rid)))
            return (rc);
      } else {         
         if ((rc = ih.InsertEntry((void *) &value, rid)))
         {
            return (rc);
         }
      }

      if ((i + 1) % PROG_UNIT == 0) {
         // cast to long for PC's
         // printf("\r\t%d%%    ", (int) ((i + 1)*100L / nEntries));
         fflush(stdout);
      }
   }
   // printf("\r\t%d%%      \n", (int) (i * 100L / nEntries));

   // Return ok
   return (0);
}

//
// Desc: Add a number of float entries to the index
//

RC InsertFloatEntries(IX_IndexHandle &ih, int nEntries) {
   RC rc;
   int i;
   float value;

   printf("             Adding %d float entries\n", nEntries);
   ran(nEntries);
   for (i = 0; i < nEntries; i++) {
      value = values[i] + 1;
      RID rid((PageNum) value, (SlotNum) value * 2);
      if ((rc = ih.InsertEntry((void *) &value, rid)))
         return (rc);

      if ((i + 1) % PROG_UNIT == 0) {
         printf("\r\t%d%%    ", (int) ((i + 1)*100L / nEntries));
         fflush(stdout);
      }
   }
   printf("\r\t%d%%      \n", (int) (i * 100L / nEntries));

   // Return ok
   return (0);
}

//
// Desc: Add a number of string entries to the index
//

RC InsertStringEntries(IX_IndexHandle &ih, int nEntries) {
   RC rc;
   int i;
   char value[STRLEN];

   printf("             Adding %d string entries\n", nEntries);
   ran(nEntries);
   for (i = 0; i < nEntries; i++) {
      memset(value, ' ', STRLEN);
      sprintf(value, "number %d", values[i] + 1);
      RID rid(values[i] + 1, (values[i] + 1)*2);
      if ((rc = ih.InsertEntry(value, rid)))
         return (rc);

      if ((i + 1) % PROG_UNIT == 0) {
         printf("\r\t%d%%    ", (int) ((i + 1)*100L / nEntries));
         fflush(stdout);
      }
   }
   printf("\r\t%d%%      \n", (int) (i * 100L / nEntries));

   // Return ok
   return (0);
}

//
// AddRecs
//
// Desc: Add a number of integer records to an RM file
//

RC AddRecs(RM_FileHandle &fh, int nRecs) {
   RC rc;
   int i;
   int value;
   RID rid;
   PageNum pageNum;
   SlotNum slotNum;

   printf("           Adding %d int records to RM file\n", nRecs);
   ran(nRecs);
   for (i = 0; i < nRecs; i++) {
      value = values[i] + 1;
      if ((rc = fh.InsertRec((char *) &value, rid)) ||
              (rc = rid.GetPageNum(pageNum)) ||
              (rc = rid.GetSlotNum(slotNum)))
         return (rc);

      if ((i + 1) % PROG_UNIT == 0) {
         printf("\r\t%d%%      ", (int) ((i + 1)*100L / nRecs));
         fflush(stdout);
      }
   }
   printf("\r\t%d%%      \n", (int) (i * 100L / nRecs));

   // Return ok
   return (0);
}

//
// DeleteIntEntries: delete a number of integer entries from an index
//

RC DeleteIntEntries(IX_IndexHandle &ih, int nEntries) {
   RC rc;
   int i;
   int value;

   printf("        Deleting %d int entries\n", nEntries);
   ran(nEntries);
   for (i = 0; i < nEntries; i++) {
      value = values[i] + 1;
      RID rid(value, value * 2);
      if ((rc = ih.DeleteEntry((void *) &value, rid)))
         return (rc);

      if ((i + 1) % PROG_UNIT == 0) {
         printf("\r\t%d%%      ", (int) ((i + 1)*100L / nEntries));
         fflush(stdout);
      }
   }
   printf("\r\t%d%%      \n", (int) (i * 100L / nEntries));

   return (0);
}

//
// DeleteFloatEntries: delete a number of float entries from an index
//

RC DeleteFloatEntries(IX_IndexHandle &ih, int nEntries) {
   RC rc;
   int i;
   float value;

   printf("        Deleting %d float entries\n", nEntries);
   ran(nEntries);
   for (i = 0; i < nEntries; i++) {
      value = values[i] + 1;
      RID rid((PageNum) value, (SlotNum) value * 2);
      if ((rc = ih.DeleteEntry((void *) &value, rid)))
         return (rc);

      if ((i + 1) % PROG_UNIT == 0) {
         printf("\r\t%d%%      ", (int) ((i + 1)*100L / nEntries));
         fflush(stdout);
      }
   }
   printf("\r\t%d%%      \n", (int) (i * 100L / nEntries));

   return (0);
}

//
// Desc: Delete a number of string entries from an index
//

RC DeleteStringEntries(IX_IndexHandle &ih, int nEntries) {
   RC rc;
   int i;
   char value[STRLEN + 1];

   printf("             Deleting %d float entries\n", nEntries);
   ran(nEntries);
   for (i = 0; i < nEntries; i++) {
      sprintf(value, "number %d", values[i] + 1);
      RID rid(values[i] + 1, (values[i] + 1)*2);
      if ((rc = ih.DeleteEntry(value, rid)))
         return (rc);

      if ((i + 1) % PROG_UNIT == 0) {
         printf("\r\t%d%%    ", (int) ((i + 1)*100L / nEntries));
         fflush(stdout);
      }
   }
   printf("\r\t%d%%      \n", (int) (i * 100L / nEntries));

   // Return ok
   return (0);
}

//
// VerifyIntIndex
//   - nStart is the starting point in the values array to check
//   - nEntries is the number of entries in the values array to check
//   - If bExists == 1, verify that an index has entries as added
//     by InsertIntEntries.
//     If bExists == 0, verify that entries do NOT exist (you can
//     use this to test deleting entries).
//

RC VerifyIntIndex(IX_IndexHandle &ih, int nStart, int nEntries, int bExists) {
   RC rc;
   int i;
   RID rid;
   IX_IndexScan scan;
   PageNum pageNum;
   SlotNum slotNum;

   // Assume values still contains the array of values inserted/deleted

   printf("Verifying index contents\n");

   for (i = nStart; i < nStart + nEntries; i++) {
      int value = values[i] + 1;

      if ((rc = scan.OpenScan(ih, EQ_OP, &value))) {
         printf("Verify error: opening scan\n");
         return (rc);
      }

      //printf("%d:scan value = %d\n",i,value);
      if(value < 170)
      {
        rc = scan.GetNextEntry(rid);
      }
      else
      {
        //printf("%d:scan value = %d\n",i,value);
        rc = scan.GetNextEntry(rid);
      }
      //printf("%d:scan value = %d,rid = (%d,%d),rc = %d,bExiste = %d\n",i,value,rid.Page(),rid.Slot(),rc,bExists);

      if (!bExists && rc == 0) {
         printf("Verify error: found non-existent entry %d\n", value);
         return (IX_EOF); // What should be returned here?
      } else if (bExists && rc == IX_EOF) {
         printf("Verify error: entry %d not found\n", value);
         return (IX_EOF); // What should be returned here?
      } else if (rc != 0 && rc != IX_EOF)
         return (rc);

      if (bExists && rc == 0) {
         // Did we get the right entry?
         if ((rc = rid.GetPageNum(pageNum)) ||
                 (rc = rid.GetSlotNum(slotNum)))
            return (rc);

         if (pageNum != value || slotNum != (value * 2)) {
            printf("Verify error: incorrect rid (%d,%d) found for entry %d\n",
                    pageNum, slotNum, value);
            return (IX_EOF); // What should be returned here?
         }

         // Is there another entry?
         rc = scan.GetNextEntry(rid);
         if (rc == 0) {
            printf("Verify error: found two entries with same value %d\n", value);
            return (IX_EOF); // What should be returned here?
         } else if (rc != IX_EOF)
            return (rc);
      }

      if ((rc = scan.CloseScan())) {
         printf("Verify error: closing scan\n");
         return (rc);
      }
   }
   return (0);
}

/////////////////////////////////////////////////////////////////////
// Sample test functions follow.                                   //
/////////////////////////////////////////////////////////////////////

//
// Test1 tests simple creation, opening, closing, and deletion of indices
//

RC Test1(void) {
   RC rc;
   int index = 0;
   IX_IndexHandle ih;

   printf("Test 1: create, open, close, delete an index... \n");
   /*
      if ((rc = ixm.CreateIndex(FILENAME, index, INT, sizeof(int))) ||
            (rc = ixm.OpenIndex(FILENAME, index, ih)) ||
            (rc = ixm.CloseIndex(ih)))
         return (rc);
    */
   if ((rc = ixm.CreateIndex(FILENAME, index, INT, sizeof (int)))) {
      printf("error in creatIndex\n");
      return (rc);
   }
   printf("IX_CreatIndex test OK!\n");
   if ((rc = ixm.OpenIndex(FILENAME, index, ih))) {
      printf("error in OpenIndex\n");
      return (rc);
   }
   printf("IX_OpenIndex test OK!\n");
   if ((rc = ixm.CloseIndex(ih))) {
      printf("error in CloseIndex\n");
      return (rc);
   }
   printf("IX_CloseIndex test OK!\n");


   LsFiles(FILENAME);

   if ((rc = ixm.DestroyIndex(FILENAME, index)))
      return (rc);

   printf("Passed Test 1\n\n");
   return (0);
}

//
// Test2 tests inserting a few integer entries into the index.
//

RC Test2(void) {
   RC rc;
   IX_IndexHandle ih;
   int index = 0;

   printf("Test2: Insert a few integer entries into an index... \n");

   //standard test
   if ((rc = ixm.CreateIndex(FILENAME, index, INT, sizeof (int))) ||
           (rc = ixm.OpenIndex(FILENAME, index, ih)) ||
           (rc = InsertIntEntries(ih, FEW_ENTRIES)) ||
           (rc = ixm.CloseIndex(ih)) ||
           (rc = ixm.OpenIndex(FILENAME, index, ih)))
      return (rc);

   //ih.PrintTree();

   // ensure inserted entries are all there
   if ((rc = VerifyIntIndex(ih, 0, FEW_ENTRIES, TRUE)))// ||
      return (rc);
   // ensure an entry not inserted is not there
   if ((rc = VerifyIntIndex(ih, FEW_ENTRIES, 1, FALSE)))
      return (rc);

   if ((rc = ixm.CloseIndex(ih)))
      return (rc);


   if ((rc = ixm.OpenIndex(FILENAME, index, ih)))
      return (rc);

   ih.PrintHeader();
   //ih.PrintTree();

   if ((rc = ixm.CloseIndex(ih)))
      return (rc);
   LsFiles(FILENAME);

   if ((rc = ixm.DestroyIndex(FILENAME, index)))
      return (rc);

   printf("Passed Test 2\n\n");
   return (0);
}

//
// Test3 tests deleting a few integer entries from an index
//

RC Test3(void) {
   RC rc;
   int index = 0;
   int nDelete = MANY_ENTRIES * 8 / 10;
   IX_IndexHandle ih;

   printf("Test3: Delete many integer entries from an index... \n");
   if ((rc = ixm.CreateIndex(FILENAME, index, INT, sizeof (int))))
   {
      return (rc);
   }

   if ((rc = ixm.OpenIndex(FILENAME, index, ih)))
   {
      return (rc);
   }
   
  // ih.getPfFileHandle()->getPfBufferMgr()->PrintBuffer();

   if ((rc = InsertIntEntries(ih, MANY_ENTRIES)))
   {
      return (rc);
   }
      
  // ih.getPfFileHandle()->getPfBufferMgr()->PrintBuffer();
  //    ih.PrintTree();
      ih.PrintHeader();
   if ((rc = ixm.CloseIndex(ih)))
      return (rc);
   if ((rc = ixm.OpenIndex(FILENAME, index, ih)))
      return (rc);
   if ((rc = VerifyIntIndex(ih, 0, MANY_ENTRIES, TRUE)))
      return (rc);
   // ensure an entry not inserted is not there
   if ((rc = VerifyIntIndex(ih, MANY_ENTRIES, 1, FALSE)))
      return (rc);
   if ((rc = ixm.CloseIndex(ih)))
      return (rc);
   printf("******************After Deleteing*****************\n");
   if ((rc = ixm.OpenIndex(FILENAME, index, ih)))
      return (rc);
   if ((rc = DeleteIntEntries(ih, nDelete)))
      return (rc);
   //ih.PrintTree();
   ih.PrintHeader();
   if ((rc = ixm.CloseIndex(ih)))
      return (rc);

   if ((rc = ixm.OpenIndex(FILENAME, index, ih)))
      return (rc);
   if ((rc = VerifyIntIndex(ih, 0, nDelete, FALSE)))
      return (rc);
   // ensure an entry not inserted is not there
   if ((rc = VerifyIntIndex(ih, nDelete, MANY_ENTRIES - nDelete, TRUE)))
      return (rc);
   if ((rc = ixm.CloseIndex(ih)))
      return (rc);

   LsFiles(FILENAME);

   if ((rc = ixm.DestroyIndex(FILENAME, index)))
      return (rc);

   printf("Passed Test 3\n\n");
   return (0);
}

//
// Test 4 tests a few inequality scans on Btree indices
//

RC Test4(void) {
   RC rc;
   IX_IndexHandle ih;
   int index = 0;
   int i;
   int value = MANY_ENTRIES / 2;
   RID rid;

   printf("Test4: Inequality scans... \n");

   if ((rc = ixm.CreateIndex(FILENAME, index, INT, sizeof (int))))
      return (rc);
   printf("insert finir.........\n");
   if ((rc = ixm.OpenIndex(FILENAME, index, ih)))
      return (rc);
   printf("insert finir.........\n");
   if ((rc = InsertIntEntries(ih, MANY_ENTRIES)))
      return (rc);
   printf("insert finir.........\n");

   //ih.PrintTree();
   // Scan <
   IX_IndexScan scanlt;
   if ((rc = scanlt.OpenScan(ih, LT_OP, &value))) {
      printf("Scan error: opening scan\n");
      return (rc);
   }
   printf("open less than scan for %d.........\n", value);

   i = 0;
   while (!(rc = scanlt.GetNextEntry(rid))) {
      i++;
   }

   if (rc != IX_EOF)
      return (rc);

   printf("Found %d entries in <-scan.\n", i);

   // Scan <=
   IX_IndexScan scanle;
   if ((rc = scanle.OpenScan(ih, LE_OP, &value))) {
      printf("Scan error: opening scan\n");
      return (rc);
   }

   printf("open less equal scan for %d.........\n", value);
   i = 0;
   while (!(rc = scanle.GetNextEntry(rid))) {
      i++;
   }
   if (rc != IX_EOF)
      return (rc);

   printf("Found %d entries in <=-scan.\n", i);

   // Scan >
   IX_IndexScan scangt;
   if ((rc = scangt.OpenScan(ih, GT_OP, &value))) {
      printf("Scan error: opening scan\n");
      return (rc);
   }
   printf("open larger than scan for %d.........\n", value);
   i = 0;
   while (!(rc = scangt.GetNextEntry(rid))) {
      i++;
   }
   if (rc != IX_EOF)
      return (rc);

   printf("Found %d entries in >-scan.\n", i);

   // Scan >=
   IX_IndexScan scange;
   if ((rc = scange.OpenScan(ih, GE_OP, &value))) {
      printf("Scan error: opening scan\n");
      return (rc);
   }

   i = 0;
   while (!(rc = scange.GetNextEntry(rid))) {
      i++;
   }
   if (rc != IX_EOF)
      return (rc);

   printf("Found %d entries in >=-scan.\n", i);

   if ((rc = ixm.CloseIndex(ih)))
      return (rc);

   LsFiles(FILENAME);

   if ((rc = ixm.DestroyIndex(FILENAME, index)))
      return (rc);

   printf("Passed Test 4\n\n");
   return (0);
}

//
// Test5 tests inserting a large integer entries into the index.
//

RC Test5(void) {
   RC rc;
   IX_IndexHandle ih;
   int index = 0;

   printf("Test5: Insert a large integer entries into an index... \n");


   //my test
   if ((rc = ixm.CreateIndex(FILENAME, index, INT, sizeof (int))) ||
           (rc = ixm.OpenIndex(FILENAME, index, ih)) ||
           (rc = InsertIntEntries(ih, MANY_ENTRIES))) {
      return (rc);
   }
   ih.PrintHeader();
   //ih.PrintTree();

   if ((rc = ixm.CloseIndex(ih))) {
      return (rc);
   }




   if ((rc = ixm.OpenIndex(FILENAME, index, ih)))
      return (rc);
   // ensure inserted entries are all there
   if ((rc = VerifyIntIndex(ih, 0, MANY_ENTRIES, TRUE)))// ||
      return (rc);
   // ensure an entry not inserted is not there
   if ((rc = VerifyIntIndex(ih, MANY_ENTRIES, 1, FALSE)))
      return (rc);

   if ((rc = ixm.CloseIndex(ih)))
      return (rc);



   LsFiles(FILENAME);

   if ((rc = ixm.DestroyIndex(FILENAME, index)))
      return (rc);

   printf("Passed Test 5\n\n");
   return (0);
}

//
// Test6 tests inserting a large float entries into the index.
//

RC Test6(void) {
   RC rc;
   IX_IndexHandle ih;
   int index = 0;

   printf("Test6: Insert a large float entries into an index... \n");


   //my test
   if ((rc = ixm.CreateIndex(FILENAME, index, INT, sizeof (int))) ||
           (rc = ixm.OpenIndex(FILENAME, index, ih)) ||
           (rc = InsertFloatEntries(ih, MANY_ENTRIES))) {
      return (rc);
   }

   if ((rc = ixm.CloseIndex(ih))) {
      return (rc);
   }

   LsFiles(FILENAME);

   if ((rc = ixm.DestroyIndex(FILENAME, index)))
      return (rc);

   printf("Passed Test 6\n\n");
   return (0);
}

//
// Test7 tests inserting a large String entries into the index.
//

RC Test7(void) {
   RC rc;
   IX_IndexHandle ih;
   int index = 0;

   printf("Test7: Insert a large String entries into an index... \n");


   //my test
   if ((rc = ixm.CreateIndex(FILENAME, index, INT, sizeof (int))) ||
           (rc = ixm.OpenIndex(FILENAME, index, ih)) ||
           (rc = InsertStringEntries(ih, MANY_ENTRIES))) {
      return (rc);
   }

   if ((rc = ixm.CloseIndex(ih))) {
      return (rc);
   }

   LsFiles(FILENAME);

   if ((rc = ixm.DestroyIndex(FILENAME, index)))
      return (rc);

   printf("Passed Test 7\n\n");
   return (0);
}
