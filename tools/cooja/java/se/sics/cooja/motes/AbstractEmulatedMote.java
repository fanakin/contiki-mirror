/*
 * Copyright (c) 2009, Swedish Institute of Computer Science. All rights
 * reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer. 2. Redistributions in
 * binary form must reproduce the above copyright notice, this list of
 * conditions and the following disclaimer in the documentation and/or other
 * materials provided with the distribution. 3. Neither the name of the
 * Institute nor the names of its contributors may be used to endorse or promote
 * products derived from this software without specific prior written
 * permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * $Id: AbstractEmulatedMote.java,v 1.3 2010/08/13 09:59:41 fros4943 Exp $
 */

package se.sics.cooja.motes;

import se.sics.cooja.Mote;
import se.sics.cooja.plugins.BufferListener;
import se.sics.cooja.plugins.BufferListener.BufferAccess;
import se.sics.cooja.plugins.TimeLine;

public abstract class AbstractEmulatedMote extends AbstractWakeupMote implements Mote {

  /**
   * @return CPU frequency (Hz)
   */
  public int getCPUFrequency() {
    return -1;
  }
  
  /**
   * @return Execution details, for instance a stack trace
   * @see TimeLine
   */
  public String getExecutionDetails() {
    return null;
  }

  /**
   * @return One-liner describing current PC, for instance source file and line.
   * May return null.
   * 
   * @see BufferListener
   */
  public String getPCString() {
    return null;
  }
  
  public interface MemoryMonitor {
    public boolean start(int address, int size);
    public void stop();
    public Mote getMote();
    public int getAddress();
    public int getSize();

    public void setLastBufferAccess(BufferAccess ba);
    public BufferAccess getLastBufferAccess();

    public boolean isPointer();
    public void setPointer(boolean isPointer, MemoryMonitor pointedMemory);
    public MemoryMonitor getPointedMemory();
  }

  public enum MemoryEventType { READ, WRITE, UNKNOWN };
  public interface MemoryEventHandler {
    public void event(MemoryMonitor mm, MemoryEventType type, int adr, int data);
  }

  public abstract MemoryMonitor createMemoryMonitor(MemoryEventHandler meh);

}
