import {EventEmitter, Injectable, isDevMode} from '@angular/core';

@Injectable({
  providedIn: 'root'
})
export class WebsocketService {

  private ws?: WebSocket;
  private emitter: EventEmitter<any> = new EventEmitter();
  private socketEventEmitter: EventEmitter<string> = new EventEmitter();
  private reconnectDesired: boolean = true;

  constructor() {
    this.initializeWebsocket();
  }

  getSocket(): WebSocket {
    return <WebSocket>this.ws;
  }

  getEmitter(): EventEmitter<any> {
    return this.emitter;
  }
  getSocketEventEmitter(): EventEmitter<string> {
    return this.socketEventEmitter;
  }



  initializeWebsocket() :void {
    const that = this;
    this.connectWS();

    // @ts-ignore
    this.ws.onopen = function() {
      that.socketEventEmitter.emit("open");
    }
    // @ts-ignore
    this.ws.onclose = () => that.handleCose();


    // @ts-ignore
    this.ws.onerror = function(evt) {
      that.emitter.emit("error")
      console.error(evt);
    }
    // @ts-ignore
    this.ws.onmessage = function (evt) {
      that.emitter.emit(JSON.parse(evt.data));
    }
  }

  cmd(c: any, p: any) {
    this.ws?.send(JSON.stringify({cmd: c, arg: p}));
  }

  private handleCose() {
    this.socketEventEmitter.emit("close");
    if (this.reconnectDesired) {
      setTimeout(() => this.initializeWebsocket(), 1000);
    }
  }

  private connectWS(): void {
    if (isDevMode()) {
      this.ws = new WebSocket('ws://localhost:9123/ws');
      return;
    }
    if (window.location.host.startsWith('https')) {
      this.ws = new WebSocket("wss://" + window.location.host + "/ws/");
    } else {
      this.ws = new WebSocket("ws://" + window.location.host + "/ws/");
    }
  }

  /**
   * Send a string to the websocket (as is).
   * @param str the string to send
   */
  public sendString(str: string) {
    this.ws?.send(str);
  }

  /**
   * Serialize the data to JSON and send it to the socket.
   * @param o the object or data to send
   */
  public sendObject(o: any) {
    this.sendString(JSON.stringify(o));
  }
}

