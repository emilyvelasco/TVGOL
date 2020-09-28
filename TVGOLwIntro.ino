// Conway's Game Of Life 128x96 using TVout
// P.Beard
// March 2013

#include <TVout.h>
#include <fontALL.h>

#define matWidth 4
#define matHeight 96

TVout TV;
unsigned long * myScreen;

void setup() {
 TV.begin(NTSC, matWidth * 32, matHeight);
 TV.select_font(font6x8);
 TV.println("       Welcome to\n");
 TV.delay(2500);
 TV.select_font(font8x8);
 TV.println("  Conway's Game\n");
 TV.println("     of Life\n");
 TV.println("   \n");
 TV.delay(2500);
 TV.clear_screen();
 
 TV.delay(1500);
 TV.println("also known\n");
 TV.delay(1500);
 TV.clear_screen();
 TV.println("simply as Life,\n");
 TV.println("is a cellular automaton\n");
 TV.delay(1500);
 TV.clear_screen();
 TV.println("devised by the\n");
 TV.println("British mathematician\n");
 TV.delay(1500);
 TV.clear_screen();
 TV.println("John Horton Conway\n");
 TV.println(" in 1970.\n");
 TV.delay(2500);
 TV.clear_screen();
 TV.println("It is a zero-player\n");
 TV.println("game, meaning.\n");
 TV.delay(2500);
 TV.clear_screen();
 TV.println("that its evolution\n");
 TV.println("is determined\n");
 TV.delay(2500);
 TV.clear_screen();
 TV.println("by its initial\n");
 TV.println("state, requiring\n");
 TV.delay(2500);
 TV.clear_screen();
 TV.println("no further input.\n");
 TV.delay(2500);
       


 
 TV.select_font(font6x8);
 TV.println("      Please enjoy\n");
 TV.delay(2500);
 TV.clear_screen();
 myScreen = (unsigned long *) TV.screen;
 randomSeed(analogRead(0));
 randomiseMatrix();
}

void loop() {
 generateMatrix();
 digitalWrite(13, !digitalRead(13));
}

unsigned long swapBytes(unsigned long x) {
 return ((x & 0x000000ffUL) << 24) | ((x & 0x0000ff00UL) << 8) | ((x & 0x00ff0000UL) >> 8) | ((x & 0xff000000UL) >> 24);
}

void randomiseMatrix() {

 //Set up initial cells in matrix
 for (int r = 0; r < matHeight; r++) {
   for (int c = 0; c < matWidth; c++) {
     myScreen[r * matWidth + c] = random(0xffff) << 16 | random(0xffff);
   }
 }
}

void injectGlider() {

 byte col = random(matWidth);
 byte row = random(matHeight);
 myScreen[(row+0) * matWidth + col] |= B0000111;
 myScreen[(row+1) * matWidth + col] |= B0000001;
 myScreen[(row+2) * matWidth + col] |= B0000010;

}
  
void generateMatrix() {
 
 //Variables holding data on neighbouring cells
 unsigned long NeighbourN[matWidth], NeighbourNW[matWidth], NeighbourNE[matWidth], CurrCells[matWidth], NeighbourW[matWidth];
 unsigned long NeighbourE[matWidth], NeighbourS[matWidth], NeighbourSW[matWidth], NeighbourSE[matWidth], firstRow[matWidth];
  
 unsigned long tot1, tot2, tot4, carry, NewCells;

 int changes = 0; // counts the changes in the matrix
 static int prevChanges = 256; // counts the changes in the matrix on prev generation
 static int staleCount = 0; // counts the consecutive occurrances of the same number of changes in the matrix

 //set up N, NW, NE, W & E neighbour data
 //also take a copy of the first row data for use later when calculating last row
 for (byte b = 0; b < matWidth; b++) {
   NeighbourN[b] = swapBytes(myScreen[(matHeight-1) * matWidth + b]);
   firstRow[b] = CurrCells[b] = swapBytes(myScreen[b]);
   
 }

 carry = NeighbourN[matWidth-1];
 for (char b = 0; b < matWidth; b++) {
   NewCells = NeighbourN[b];
   NeighbourNW[b] = NewCells >> 1 | carry << 31;
   carry = NewCells;
 }
 
 carry = NeighbourN[0];    
 for (char b = matWidth-1; b >= 0; b--) {
   NewCells = NeighbourN[b];
   NeighbourNE[b] = NewCells << 1 | carry >> 31;
   carry = NewCells;
 }
  
 carry = CurrCells[matWidth-1];
 for (char b = 0; b < matWidth; b++) {
   NewCells = CurrCells[b];
   NeighbourW[b] = NewCells >> 1 | carry << 31;
   carry = NewCells;
 }
 
 carry = CurrCells[0];    
 for (char b = matWidth-1; b >= 0; b--) {
   NewCells = CurrCells[b];
   NeighbourE[b] = NewCells << 1 | carry >> 31;
   carry = NewCells;
 }
 
 //Process each row of the matrix
 for (byte row = 0; row < matHeight; row++) {
    
   //Pick up new S, SW & SE neighbours
   if (row < matHeight - 1) {
     for (byte b = 0; b < matWidth; b++) {
       NeighbourS[b] = swapBytes(myScreen[(row+1) * matWidth + b]);
     }
   }
   else {
     for (byte b = 0; b < matWidth; b++) {
       NeighbourS[b] = firstRow[b];
     }
   }
 
   carry = NeighbourS[matWidth-1];
   for (char b = 0; b < matWidth; b++) {
     NewCells = NeighbourS[b];
     NeighbourSW[b] = NewCells >> 1 | carry << 31;
     carry = NewCells;
   }
     
   carry = NeighbourS[0];    
   for (char b = matWidth-1; b >= 0; b--) {
     NewCells = NeighbourS[b];
     NeighbourSE[b] = NewCells << 1 | carry >> 31;
     carry = NewCells;
   }
 
   for (char b = 0; b < matWidth; b++) {
     
      //Count the live neighbours (in parallel) for the current row of cells
     //However, if total goes over 3, we don't care (see below), so counting stops at 4
     tot1 = NeighbourN[b];
     tot2 = tot1 & NeighbourNW[b]; tot1 = tot1 ^ NeighbourNW[b];
     carry = tot1 & NeighbourNE[b]; tot1 = tot1 ^ NeighbourNE[b]; tot4 = tot2 & carry; tot2 = tot2 ^ carry;
     carry = tot1 & NeighbourW[b]; tot1 = tot1 ^ NeighbourW[b]; tot4 = tot2 & carry | tot4; tot2 = tot2 ^ carry;
     carry = tot1 & NeighbourE[b]; tot1 = tot1 ^ NeighbourE[b]; tot4 = tot2 & carry | tot4; tot2 = tot2 ^ carry;
     carry = tot1 & NeighbourS[b]; tot1 = tot1 ^ NeighbourS[b]; tot4 = tot2 & carry | tot4; tot2 = tot2 ^ carry;
     carry = tot1 & NeighbourSW[b]; tot1 = tot1 ^ NeighbourSW[b]; tot4 = tot2 & carry | tot4; tot2 = tot2 ^ carry;
     carry = tot1 & NeighbourSE[b]; tot1 = tot1 ^ NeighbourSE[b]; tot4 = tot2 & carry | tot4; tot2 = tot2 ^ carry;
    
     //Calculate the updated cells:
     // <2 or >3 neighbours, cell dies
     // =2 neighbours, cell continues to live
     // =3 neighbours, new cell born
     NewCells = (CurrCells[b] | tot1) & tot2 & ~ tot4;
     
     //Have any cells changed?
     if (NewCells != CurrCells[b]) {
       myScreen[row * matWidth + b] = swapBytes(NewCells);
       //Count the change for "stale" test
       changes++;
     }
     
     //Current cells (before update), E , W, SE, SW and S neighbours become
     //new N, NW, NE, E, W neighbours and current cells for next loop
     NeighbourN[b] = CurrCells[b];
     NeighbourNW[b] = NeighbourW[b];
     NeighbourNE[b] = NeighbourE[b];
     NeighbourE[b] = NeighbourSE[b];
     NeighbourW[b] = NeighbourSW[b];
     CurrCells[b] = NeighbourS[b];
   } //next col
 } //next row
   
 if (changes != prevChanges) staleCount = 0; else staleCount++; //Detect "stale" matrix
 if (staleCount > 16) injectGlider(); //Inject a glider

 prevChanges = changes;
 delay(100);
}
