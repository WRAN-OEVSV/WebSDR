import { Component } from '@angular/core';
import {WebsocketService} from "../websocket.service";

class NeighbourCacheEntry {
  constructor(public mac: string, public ipv4: Array<string>, public ipv6: Array<string>,
              public callsign: string, public comment: string) {
  }
}
@Component({
  selector: 'app-neighbor-cache',
  templateUrl: './neighbor-cache.component.html',
  styleUrls: ['./neighbor-cache.component.css']
})
export class NeighborCacheComponent {

  neighbourCache : Array<NeighbourCacheEntry> = []

  constructor(private websocket: WebsocketService) {
    websocket.getEmitter().subscribe((data) => data['neighbor-cache'] && this.processData(data));
    websocket.getSocketEventEmitter().subscribe(code => this.socketEvent(code));
    this.updateNeighbourCache();
  }

  private socketEvent(code: string) {
    if ('authenticated' == code) {
      this.updateNeighbourCache();
    }
  }

  private updateNeighbourCache() {
    this.websocket.sendObject({cmd: "neighbor_cache", sub_cmd: "list"});
  }

  private processData(data: any) {
    const unpacked = data['neighbor-cache'];
    if (!!unpacked['list'] && Array.isArray(unpacked['list'])) {
      this.neighbourCache = [];
      for (const o of unpacked['list']) {
        // @ts-ignore
        this.neighbourCache.push(new NeighbourCacheEntry(o['MAC'], o['IPv4'], o['IPv6'], o['callsign'], o['comment']));
      }
    }
  }
}
