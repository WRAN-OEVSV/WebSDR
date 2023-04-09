import { ComponentFixture, TestBed } from '@angular/core/testing';

import { NeighborCacheComponent } from './neighbor-cache.component';

describe('NeighbourCacheComponent', () => {
  let component: NeighborCacheComponent;
  let fixture: ComponentFixture<NeighborCacheComponent>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      declarations: [ NeighborCacheComponent ]
    })
    .compileComponents();

    fixture = TestBed.createComponent(NeighborCacheComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
