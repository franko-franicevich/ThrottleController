

/* 
 * Convert uint16 to a formatted binary string with 'X' representing 1,
 * and '.' representing 0. 
 * We use these rather than 1's and 0s to make it easier to spot the difference.
 * bufferLength should be enough to include the null terminator
 */
void uint16_to_binary_string( uint16_t value, char *buffer, uint8_t bufferLength)
{
  // We're going to start with the least significant bit, 
  // and write it at the end of the buffer, then count down.
  int len = min(16, bufferLength-1);
  buffer[len] = 0; // Ensure null termination.
  for (int i = 0; i < len; i++) {
    if (value & (1 << i)) {
      buffer[len - i - 1] = 'X';
    } else {
      buffer[len - i - 1] = '.';
    }
  }
}
