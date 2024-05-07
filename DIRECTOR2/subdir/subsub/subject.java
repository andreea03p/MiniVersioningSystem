abstract class Avion
{
    protected String planeID;
    protected int totalEnginePower;
    
    public Avion(String id, int engine)
    {
        this.planeID = id;
        this.totalEnginePower = engine;
    }
    
    public String getPlaneID()
    {
        return this.planeID;
    }
    
    public int getTotalEnginePower()
    {
        return this.totalEnginePower;
    } 
    
    public void takeOff()
    {
        System.out.println("PlaneID " + this.planeID + " - Initiating takeoff procedure - Starting engines - Accelerating down the runway - Taking off - Retracting gear - Takeoff complete");
    }
    
    public void land()
    {
        System.out.println("PlaneID " + this.planeID + " - Initiating landing procedure - Enabling airbrakes - Lowering gear - Contacting runway - Decelerating - Stopping engines - Landing complete");
    }
    
    public void fly()
    {
        System.out.println("PlaneID " + this.planeID + " - Flying");
    }
}


abstract class AvionCalatori extends Avion
{
    protected int maxPassengers;
    
    public AvionCalatori(String id, int engine, int maxPass)
    {
        super(id, engine);
        this.maxPassengers = maxPass;
    }
    
    public int getMaxPassengers()
    {
        return this.maxPassengers;
    }
}

class AvionBoeing extends AvionCalatori
{
    public AvionBoeing(String id, int engine, int maxPass)
    {
        super(id, engine, maxPass);
    }
}

class AvionConcorde extends AvionCalatori
{
    public AvionConcorde(String id, int engine, int maxPass)
    {
        super(id, engine, maxPass);
    }
    
    public void goSuperSonic()
    {
        System.out.println("PlaneID " + super.planeID + " - Supersonic mode activated");
    }
    
    public void goSubSonic()
    {
        System.out.println("PlaneID " + super.planeID + " - Supersonic mode deactivated");
    }
}


abstract class AvionLupta extends Avion
{
    public AvionLupta(String id, int engine)
    {
        super(id, engine);
    }
    
    public void LaunchMissile()
    {
        System.out.println("PlaneID: " + super.planeID + " - Initiating missile launch procedure - Acquiring target - Launching missile - Breaking away - Missile launch complete");
    }
}

class AvionMig extends AvionLupta
{
    public AvionMig(String id, int engine)
    {
        super(id, engine);
    }
    
    public void highSpeedGeometry()
    {
        System.out.println("PlaneID: " + super.planeID + " - High speed geometry selected");
    }
    
    public void normalGeometry()
    {
        System.out.println("PlaneID: " + super.planeID + " - Normal geometry selected");
    }
}

class AvionTomCat extends AvionLupta
{
    public AvionTomCat(String id, int engine)
    {
        super(id, engine);
    }
    
    public void refuel()
    {
        System.out.println("PlaneID: " + super.planeID + " - Initiating refueling procedure - Locating refueller - Catching up - Refueling - Refueling complete");
    }
}


public class MainPlane
{
    public static void main(String[] args)
    {
        Avion a1 = new AvionBoeing("1111", 2, 200);
        Avion a2 = new AvionConcorde("2222", 2, 300);
        Avion a3 = new AvionConcorde("3333", 2, 400);
        
        if(a3 instanceof AvionConcorde)
        {
            AvionConcorde temp = (AvionConcorde) a3;
            temp.goSubSonic();
        }
        
        if(a2 instanceof AvionConcorde)
        {
            AvionConcorde temp = (AvionConcorde) a2;
            temp.goSuperSonic();
        }
        
        Avion b1 = new AvionMig("4444", 50);
        Avion b2 = new AvionMig("5555", 40);
        Avion b3 = new AvionTomCat("7777", 10);
        
        if(b1 instanceof AvionMig)
        {
            AvionMig temp = (AvionMig) b1;
            temp.highSpeedGeometry();
        }
        
        if(b2 instanceof AvionMig)
        {
            AvionMig temp = (AvionMig) b2;
            temp.normalGeometry();
        }
        
        if(b3 instanceof AvionTomCat)
        {
            AvionTomCat temp = (AvionTomCat) b3;
            temp.refuel();
        }
        
        b1.takeOff();
        a1.land();
        a2.fly();
    }
}



