import { ComponentFixture, TestBed } from '@angular/core/testing';

import { NeighbourCacheComponent } from './neighbour-cache.component';

describe('NeighbourCacheComponent', () => {
  let component: NeighbourCacheComponent;
  let fixture: ComponentFixture<NeighbourCacheComponent>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      declarations: [ NeighbourCacheComponent ]
    })
    .compileComponents();

    fixture = TestBed.createComponent(NeighbourCacheComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
