#include <iostream>
#include <list>
#include <set>
#include <queue>
#include <sstream>
#include <chrono>

// 是否生成调试字符串
#define DEBUG_STRING 1


// 将x,y坐标顺时针旋转到rx,ry坐标
#define ROTATE_0(x, y, rx, ry, originWidth, originHeight) \
    rx = x; \
    ry = y;

#define ROTATE_90(x, y, rx, ry, originWidth, originHeight) \
    rx = y; \
    ry = originWidth - (x) - 1;

#define ROTATE_180(x, y, rx, ry, originWidth, originHeight) \
    rx = originWidth - (x) - 1; \
    ry = originHeight - (y) - 1;

#define ROTATE_270(x, y, rx, ry, originWidth, originHeight) \
    rx = originHeight - (y) - 1; \
    ry = x;



// 将x,y坐标逆时针旋转到rx,ry坐标
#define INVROTATE_0(x, y, rx, ry, originWidth, originHeight) \
    rx = x; \
    ry = y;

#define INVROTATE_90(x, y, rx, ry, originWidth, originHeight) \
    rx = y; \
    ry = originHeight - (x) - 1;

#define INVROTATE_180(x, y, rx, ry, originWidth, originHeight) \
    rx = originWidth - (x) - 1; \
    ry = originHeight - (y) - 1;

#define INVROTATE_270(x, y, rx, ry, originWidth, originHeight) \
    rx = originWidth - (y) - 1; \
    ry = x;



// 顺时针旋转
enum class Rotation
{
    _0,
    _90,
    _180,
    _270,
};

class GridGroup;

class Grid
{
public:
    Grid()
    {
    }

    ~Grid()
    {
        //if (m_SubGrid != NULL)
        //    throw new std::exception("子格子不为空");
    }

    void SetOwner(GridGroup* owner)
    {
        m_Owner = owner;
    }

    GridGroup* GetOwner() const
    {
        return m_Owner;
    }

    inline void SetSubGrid(Grid* grid)
    {
        m_SubGrid = grid;
    }

    Grid* GetSubGrid() const
    {
        return m_SubGrid;
    }

private:
    GridGroup* m_Owner = NULL; // 拥有者
    Grid* m_SubGrid = NULL; // 所属放置的格子
};

// 格子组
class GridGroup
{
public:
    GridGroup()
    {
    }
    
    GridGroup(int width, int height)
    {
        SetSize(width, height);
    }

    ~GridGroup()
    {
        Destroy();
    }

    void Destroy()
    {
        if (m_Grids != NULL)
        {
            for (auto i = 0; i < m_Width; i++)
            {
                delete[] m_Grids[i];
            }
            delete[] m_Grids;
        }
        m_Grids = NULL;
        m_Width = 0;
        m_Height = 0;
    }

    void SetId(int id)
    {
        m_Id = id;
    }

    int GetId() const
    {
        return m_Id;
    }

    int GetWidth() const
    {
        return m_Width;
    }

    int GetHeight() const
    {
        return m_Height;
    }

    void AddGrid(int x, int y, Grid* grid)
    {
        SetGrid(x, y, grid);
        grid->SetOwner(this);
    }

    void SetGrid(int x, int y, Grid* grid)
    {
        if (grid != NULL)
        {
            if (x >= m_Width || y >= m_Height)
            {
                SetSize(x + 1, y + 1);
            }

            if (m_Grids[x][y] == NULL)
            {
                m_Grids[x][y] = grid;
                m_RecheckRotationValid = true;
                m_VaildGridCount++;

            }
            else
            {
                std::stringstream ss;
                ss << "无法设置格子，已有其他格子: " << "(" << x << "," << y << ")";
                std::string formatted = ss.str();
                throw new std::exception(formatted.c_str());
            }
        }
        else
        {
            if (x < m_Width && y < m_Height && m_Grids[x][y] != NULL)
            {
                m_Grids[x][y] = NULL;
                m_RecheckRotationValid = true;
                m_VaildGridCount--;
            }
        }
    }

    // 顺时针旋转
    void GetRotationPos(int x, int y, Rotation r, int& rx, int& ry) const
    {
        if (r == Rotation::_0)
        {
            ROTATE_0(x, y, rx, ry, m_Width, m_Height);
        }
        else if (r == Rotation::_90)
        {
            ROTATE_90(x, y, rx, ry, m_Width, m_Height);
        }
        else if (r == Rotation::_180)
        {
            ROTATE_180(x, y, rx, ry, m_Width, m_Height);
        }
        else if (r == Rotation::_270)
        {
            ROTATE_270(x, y, rx, ry, m_Width, m_Height);
        }
        else
        {
            std::stringstream ss;
            ss << "非法旋转: " << (int)r;
            std::string formatted = ss.str();
            throw new std::exception(formatted.c_str());
        }
    }

    // 逆时针旋转
    void GetInvRotationPos(int x, int y, Rotation r, int& rx, int& ry) const
    {
        if (r == Rotation::_0)
        {
            INVROTATE_0(x, y, rx, ry, m_Width, m_Height);
        }
        else if (r == Rotation::_90)
        {
            INVROTATE_90(x, y, rx, ry, m_Width, m_Height);
        }
        else if (r == Rotation::_180)
        {
            INVROTATE_180(x, y, rx, ry, m_Width, m_Height);
        }
        else if (r == Rotation::_270)
        {
            INVROTATE_270(x, y, rx, ry, m_Width, m_Height);
        }
        else
        {
            std::stringstream ss;
            ss << "非法旋转: " << (int)r;
            std::string formatted = ss.str();
            throw new std::exception(formatted.c_str());
        }
    }

    Grid* GetGrid(int x, int y) const
    {
        if (x >= 0 && x < m_Width && y >= 0 && y < m_Height)
            return m_Grids[x][y];
        else
            return NULL;
    }

    Grid*** GetGrids() const
    {
        return m_Grids;
    }

    void SetMoveable(bool v)
    {
        m_Moveable = v;
    }

    bool GetMoveable() const
    {
        return m_Moveable;
    }

    void SetRotatable(bool v)
    {
        m_Rotatable = v;
    }

    bool GetRotatable() const
    {
        return m_Rotatable;
    }

    void CheckRotationValid()
    {
        m_RotationValid[0] = true; // 0旋转一定有效
        m_RotationValid[1] = false;
        m_RotationValid[2] = false;
        m_RotationValid[3] = false;
        for (int x = 0; x < m_Width; x++)
        {
            for (int y = 0; y < m_Height; y++)
            {
                int _90x, _90y;
                GetRotationPos(x, y, Rotation::_90, _90x, _90y);
                int _180x, _180y;
                GetRotationPos(x, y, Rotation::_180, _180x, _180y);
                if (GetGrid(x, y) != NULL)
                {
                    m_RotationValid[1] |= GetGrid(_90x, _90y) == NULL; // x, y有grid，但90旋转的位置没有grid，那么就需要90旋转
                    m_RotationValid[2] |= GetGrid(_180x, _180y) == NULL; // x, y有grid，但180旋转的位置没有grid，那么就需要180旋转
                }
                else
                {
                    m_RotationValid[1] |= GetGrid(_90x, _90y) != NULL; // x, y没有grid，但90旋转的位置有grid，那么就需要90旋转
                    m_RotationValid[2] |= GetGrid(_180x, _180y) != NULL; // x, y没有grid，但180旋转的位置有grid，那么就需要180旋转
                }
                m_RotationValid[3] = m_RotationValid[2]; // 如果旋转180有必要，那么270也有必要，因为0到180，等价于90到270

                if (m_RotationValid[0] && m_RotationValid[1] && m_RotationValid[2] && m_RotationValid[3])
                    return; // 全部有必要，返回
            }
        }
    }

    const bool* GetRotationValid()
    {
        if (m_RecheckRotationValid)
        {
            m_RecheckRotationValid = false;
            CheckRotationValid();
        }
        return m_RotationValid;
    }

    int GetVaildGridCount() const
    {
        return m_VaildGridCount;
    }

private:
    int m_Id = -1; // id
    int m_Width = 0; // 原始宽
    int m_Height = 0; // 原始高
    Grid*** m_Grids = NULL; // 格子
    bool m_Moveable = true; // 是否可移动
    bool m_Rotatable = true; // 是否可旋转
    bool m_RecheckRotationValid = false; // 是否需要重新检查旋转有效性
    bool m_RotationValid[4] = { true, false, false, false }; // 0, 90, 180, 270旋转是否有意义; 0一定为true
    int m_VaildGridCount = 0; // 有效格子数量

    // width和height只能扩大，不能缩小
    void SetSize(int newWidth, int newHeight)
    {
        if (newWidth > m_Width || newHeight > m_Height)
        {
            if (newWidth < m_Width)
                newWidth = m_Width;
            if (newHeight < m_Height)
                newHeight = m_Height;
            auto newGrids = new Grid**[newWidth]{ NULL };
            for (auto i = 0; i < newWidth; i++)
                newGrids[i] = new Grid*[newHeight]{ NULL };
            if (m_Width * m_Height > 0)
            {
                for (auto i = 0; i < m_Width; i++)
                    std::memcpy(newGrids[i], m_Grids[i], m_Height * sizeof(Grid*));
            }
            Destroy();
            m_Grids = newGrids;
            m_Width = newWidth;
            m_Height = newHeight;
        }
    }
};

class GridGroupTransform
{
public:
    GridGroupTransform(GridGroup* gridGroup)
    {
        m_GridGroup = gridGroup;
        SetRotate(Rotation::_0);
    }

    ~GridGroupTransform()
    {
        if (m_DebugString != NULL)
            delete m_DebugString;
        m_DebugString = NULL;
    }

    void SetPos(int posX, int posY)
    {
        m_PosX = posX;
        m_PosY = posY;
    }

    int GetPosX() const
    {
        return m_PosX;
    }

    int GetPosY() const
    {
        return m_PosY;
    }

    void SetRotate(Rotation r)
    {
        m_Rotation = r;
        RebuildDebugString();
        GenDebugString();
    }

    Rotation GetRotate() const
    {
        return m_Rotation;
    }

    GridGroup* GetGridGroup() const
    {
        return m_GridGroup;
    }

    int GetWidth() const
    {
        if (m_Rotation == Rotation::_0 || m_Rotation == Rotation::_180)
            return m_GridGroup->GetWidth();
        else
            return m_GridGroup->GetHeight();
    }

    int GetHeight() const
    {
        if (m_Rotation == Rotation::_0 || m_Rotation == Rotation::_180)
            return m_GridGroup->GetHeight();
        else
            return m_GridGroup->GetWidth();
    }

    // posX和posY是世界坐标，因此下面要做逆移动和逆旋转
    Grid* GetGrid(int worldPosX, int worldPosY) const
    {
        int x, y;
        m_GridGroup->GetInvRotationPos(worldPosX - m_PosX, worldPosY - m_PosY, m_Rotation, x, y);
        return m_GridGroup->GetGrid(x, y);
    }

    // posX和posY是世界坐标，因此下面要做逆移动和逆旋转
    Grid* GetGridLocal(int lx, int ly) const
    {
        int x, y;
        m_GridGroup->GetInvRotationPos(lx, ly, m_Rotation, x, y);
        return m_GridGroup->GetGrid(x, y);
    }

    // 判断trans是否与本transform的格子重叠
    bool IsOverlapTransform(GridGroupTransform* trans) const
    {
        int transPosX = trans->GetPosX();
        int transPosY = trans->GetPosY();
        int transWidth = trans->GetGridGroup()->GetWidth();
        int transHeight = trans->GetGridGroup()->GetHeight();
        auto transGrids = trans->GetGridGroup()->GetGrids();

        int myPosX = GetPosX();
        int myPosY = GetPosY();
        int myWidth = GetWidth();
        int myHeight = GetHeight();
        auto myGrids = GetGridGroup()->GetGrids();

        int x, y;

        if (trans->GetRotate() == Rotation::_0)
        {
            for (auto _x = transPosX; _x < transPosX + transWidth; _x++)
            {
                for (auto _y = transPosY; _y < transPosY + transHeight; _y++)
                {
                    INVROTATE_0(_x - transPosX, _y - transPosY, x, y, transWidth, transHeight);
                    auto transGrid = transGrids[x][y];
                    if (transGrid != NULL)
                    {
                        x = _x - myPosX;
                        y = _y - myPosY;
                        if (x >= 0 && x < myWidth && y >= 0 && y < myHeight)
                        {
                            auto grid = myGrids[x][y];
                            if (grid == NULL || grid->GetSubGrid() != NULL)
                                // 这个位置已经有其他格子了，失败
                                return true;
                        }
                        else
                            return true;
                    }
                }
            }
        }
        else if (trans->GetRotate() == Rotation::_90)
        {
            for (auto _x = transPosX; _x < transPosX + transHeight; _x++)
            {
                for (auto _y = transPosY; _y < transPosY + transWidth; _y++)
                {
                    INVROTATE_90(_x - transPosX, _y - transPosY, x, y, transWidth, transHeight);
                    auto transGrid = transGrids[x][y];
                    if (transGrid != NULL)
                    {
                        x = _x - myPosX;
                        y = _y - myPosY;
                        if (x >= 0 && x < myWidth && y >= 0 && y < myHeight)
                        {
                            auto grid = myGrids[x][y];
                            if (grid == NULL || grid->GetSubGrid() != NULL)
                                // 这个位置已经有其他格子了，失败
                                return true;
                        }
                        else
                            return true;
                    }
                }
            }
        }
        else if (trans->GetRotate() == Rotation::_180)
        {
            for (auto _x = transPosX; _x < transPosX + transWidth; _x++)
            {
                for (auto _y = transPosY; _y < transPosY + transHeight; _y++)
                {
                    INVROTATE_180(_x - transPosX, _y - transPosY, x, y, transWidth, transHeight);
                    auto transGrid = transGrids[x][y];
                    if (transGrid != NULL)
                    {
                        x = _x - myPosX;
                        y = _y - myPosY;
                        if (x >= 0 && x < myWidth && y >= 0 && y < myHeight)
                        {
                            auto grid = myGrids[x][y];
                            if (grid == NULL || grid->GetSubGrid() != NULL)
                                // 这个位置已经有其他格子了，失败
                                return true;
                        }
                        else
                            return true;
                    }
                }
            }
        }
        else if (trans->GetRotate() == Rotation::_270)
        {
            for (auto _x = transPosX; _x < transPosX + transHeight; _x++)
            {
                for (auto _y = transPosY; _y < transPosY + transWidth; _y++)
                {
                    INVROTATE_270(_x - transPosX, _y - transPosY, x, y, transWidth, transHeight);
                    auto transGrid = transGrids[x][y];
                    if (transGrid != NULL)
                    {
                        x = _x - myPosX;
                        y = _y - myPosY;
                        if (x >= 0 && x < myWidth && y >= 0 && y < myHeight)
                        {
                            auto grid = myGrids[x][y];
                            if (grid == NULL || grid->GetSubGrid() != NULL)
                                // 这个位置已经有其他格子了，失败
                                return true;
                        }
                        else
                            return true;
                    }
                }
            }
        }
        return false;
    }

    // 判断trans是否与本transform的格子相邻
    bool IsTouchTransform(GridGroupTransform* trans) const
    {
        for (auto x = trans->GetPosX(); x < trans->GetPosX() + trans->GetWidth(); x++)
        {
            for (auto y = trans->GetPosY(); y < trans->GetPosY() + trans->GetHeight(); y++)
            {
                // 这个位置自己有格子，本transform没有格子，且上下左右有格子，那么就算是相邻
                if (trans->GetGrid(x, y) != NULL)
                {
                    auto grid = GetGrid(x, y);
                    if (grid != NULL && grid->GetSubGrid() == NULL)
                    {
                        grid = GetGrid(x - 1, y);
                        if (grid != NULL && grid->GetSubGrid() != NULL)
                            return true;
                        else
                        {
                            grid = GetGrid(x + 1, y);
                            if (grid != NULL && grid->GetSubGrid() != NULL)
                                return true;
                            else
                            {
                                grid = GetGrid(x, y + 1);
                                if (grid != NULL && grid->GetSubGrid() != NULL)
                                    return true;
                                else
                                {
                                    grid = GetGrid(x, y - 1);
                                    if (grid != NULL && grid->GetSubGrid() != NULL)
                                        return true;
                                    else
                                        return false;
                                }
                            }
                        }
                    }
                }
            }
        }
        return false;
    }

    bool TryAddTransform(GridGroupTransform* trans)
    {
        int transPosX = trans->GetPosX();
        int transPosY = trans->GetPosY();
        int transWidth = trans->GetGridGroup()->GetWidth();
        int transHeight = trans->GetGridGroup()->GetHeight();
        auto transGrids = trans->GetGridGroup()->GetGrids();

        int myPosX = GetPosX();
        int myPosY = GetPosY();
        int myWidth = GetWidth();
        int myHeight = GetHeight();
        auto myGrids = GetGridGroup()->GetGrids();

        int x, y;

        m_ChildrenCount++;

        // 将trans放入
        if (trans->GetRotate() == Rotation::_0)
        {
            for (auto _x = transPosX; _x < transPosX + transWidth; _x++)
            {
                for (auto _y = transPosY; _y < transPosY + transHeight; _y++)
                {
                    INVROTATE_0(_x - transPosX, _y - transPosY, x, y, transWidth, transHeight);
                    auto transGrid = transGrids[x][y];
                    if (transGrid != NULL)
                    {
                        x = _x - myPosX;
                        y = _y - myPosY;
                        if (x >= 0 && x < myWidth && y >= 0 && y < myHeight)
                        {
                            auto grid = myGrids[x][y];
                            // 这个位置不能放格子，或者已经被占据了，失败
                            if (grid == NULL || grid->GetSubGrid() != NULL)
                            {
                                TryRemoveTransform(trans);
                                return false;
                            }
                            grid->SetSubGrid(transGrid);
                            m_RemainVaildGridCount--;
                        }
                        else
                        {
                            TryRemoveTransform(trans);
                            return false;
                        }
                    }
                }
            }
        }
        else if (trans->GetRotate() == Rotation::_90)
        {
            for (auto _x = transPosX; _x < transPosX + transHeight; _x++)
            {
                for (auto _y = transPosY; _y < transPosY + transWidth; _y++)
                {
                    INVROTATE_90(_x - transPosX, _y - transPosY, x, y, transWidth, transHeight);
                    auto transGrid = transGrids[x][y];
                    if (transGrid != NULL)
                    {
                        x = _x - myPosX;
                        y = _y - myPosY;
                        if (x >= 0 && x < myWidth && y >= 0 && y < myHeight)
                        {
                            auto grid = myGrids[x][y];
                            // 这个位置不能放格子，或者已经被占据了，失败
                            if (grid == NULL || grid->GetSubGrid() != NULL)
                            {
                                TryRemoveTransform(trans);
                                return false;
                            }
                            grid->SetSubGrid(transGrid);
                            m_RemainVaildGridCount--;
                        }
                        else
                        {
                            TryRemoveTransform(trans);
                            return false;
                        }
                    }
                }
            }
        }
        else if (trans->GetRotate() == Rotation::_180)
        {
            for (auto _x = transPosX; _x < transPosX + transWidth; _x++)
            {
                for (auto _y = transPosY; _y < transPosY + transHeight; _y++)
                {
                    INVROTATE_180(_x - transPosX, _y - transPosY, x, y, transWidth, transHeight);
                    auto transGrid = transGrids[x][y];
                    if (transGrid != NULL)
                    {
                        x = _x - myPosX;
                        y = _y - myPosY;
                        if (x >= 0 && x < myWidth && y >= 0 && y < myHeight)
                        {
                            auto grid = myGrids[x][y];
                            // 这个位置不能放格子，或者已经被占据了，失败
                            if (grid == NULL || grid->GetSubGrid() != NULL)
                            {
                                TryRemoveTransform(trans);
                                return false;
                            }
                            grid->SetSubGrid(transGrid);
                            m_RemainVaildGridCount--;
                        }
                        else
                        {
                            TryRemoveTransform(trans);
                            return false;
                        }
                    }
                }
            }
        }
        else if (trans->GetRotate() == Rotation::_270)
        {
            for (auto _x = transPosX; _x < transPosX + transHeight; _x++)
            {
                for (auto _y = transPosY; _y < transPosY + transWidth; _y++)
                {
                    INVROTATE_270(_x - transPosX, _y - transPosY, x, y, transWidth, transHeight);
                    auto transGrid = transGrids[x][y];
                    if (transGrid != NULL)
                    {
                        x = _x - myPosX;
                        y = _y - myPosY;
                        if (x >= 0 && x < myWidth && y >= 0 && y < myHeight)
                        {
                            auto grid = myGrids[x][y];
                            // 这个位置不能放格子，或者已经被占据了，失败
                            if (grid == NULL || grid->GetSubGrid() != NULL)
                            {
                                TryRemoveTransform(trans);
                                return false;
                            }
                            grid->SetSubGrid(transGrid);
                            m_RemainVaildGridCount--;
                        }
                        else
                        {
                            TryRemoveTransform(trans);
                            return false;
                        }
                    }
                }
            }
        }

        GenDebugString();

        return true;
    }

    bool TryRemoveTransform(GridGroupTransform* trans)
    {
        int transPosX = trans->GetPosX();
        int transPosY = trans->GetPosY();
        int transWidth = trans->GetGridGroup()->GetWidth();
        int transHeight = trans->GetGridGroup()->GetHeight();
        auto transGrids = trans->GetGridGroup()->GetGrids();

        int myPosX = GetPosX();
        int myPosY = GetPosY();
        int myWidth = GetWidth();
        int myHeight = GetHeight();
        auto myGrids = GetGridGroup()->GetGrids();

        int x, y;

        if (m_ChildrenCount == 0)
            return false;

        // 将trans移除
        if (trans->GetRotate() == Rotation::_0)
        {
            for (auto _x = transPosX; _x < transPosX + transWidth; _x++)
            {
                for (auto _y = transPosY; _y < transPosY + transHeight; _y++)
                {
                    INVROTATE_0(_x - transPosX, _y - transPosY, x, y, transWidth, transHeight);
                    auto transGrid = transGrids[x][y];
                    if (transGrid != NULL)
                    {
                        x = _x - myPosX;
                        y = _y - myPosY;
                        if (x >= 0 && x < myWidth && y >= 0 && y < myHeight)
                        {
                            auto grid = myGrids[x][y];
                            if (grid != NULL)
                            {
                                grid->SetSubGrid(NULL);
                                m_RemainVaildGridCount++;
                            }
                        }
                    }
                }
            }
        }
        else if (trans->GetRotate() == Rotation::_90)
        {
            for (auto _x = transPosX; _x < transPosX + transHeight; _x++)
            {
                for (auto _y = transPosY; _y < transPosY + transWidth; _y++)
                {
                    INVROTATE_90(_x - transPosX, _y - transPosY, x, y, transWidth, transHeight);
                    auto transGrid = transGrids[x][y];
                    if (transGrid != NULL)
                    {
                        x = _x - myPosX;
                        y = _y - myPosY;
                        if (x >= 0 && x < myWidth && y >= 0 && y < myHeight)
                        {
                            auto grid = myGrids[x][y];
                            if (grid != NULL)
                            {
                                grid->SetSubGrid(NULL);
                                m_RemainVaildGridCount++;
                            }
                        }
                    }
                }
            }
        }
        else if (trans->GetRotate() == Rotation::_180)
        {
            for (auto _x = transPosX; _x < transPosX + transWidth; _x++)
            {
                for (auto _y = transPosY; _y < transPosY + transHeight; _y++)
                {
                    INVROTATE_180(_x - transPosX, _y - transPosY, x, y, transWidth, transHeight);
                    auto transGrid = transGrids[x][y];
                    if (transGrid != NULL)
                    {
                        x = _x - myPosX;
                        y = _y - myPosY;
                        if (x >= 0 && x < myWidth && y >= 0 && y < myHeight)
                        {
                            auto grid = myGrids[x][y];
                            if (grid != NULL)
                            {
                                grid->SetSubGrid(NULL);
                                m_RemainVaildGridCount++;
                            }
                        }
                    }
                }
            }
        }
        else if (trans->GetRotate() == Rotation::_270)
        {
            for (auto _x = transPosX; _x < transPosX + transHeight; _x++)
            {
                for (auto _y = transPosY; _y < transPosY + transWidth; _y++)
                {
                    INVROTATE_270(_x - transPosX, _y - transPosY, x, y, transWidth, transHeight);
                    auto transGrid = transGrids[x][y];
                    if (transGrid != NULL)
                    {
                        x = _x - myPosX;
                        y = _y - myPosY;
                        if (x >= 0 && x < myWidth && y >= 0 && y < myHeight)
                        {
                            auto grid = myGrids[x][y];
                            if (grid != NULL)
                            {
                                grid->SetSubGrid(NULL);
                                m_RemainVaildGridCount++;
                            }
                        }
                    }
                }
            }
        }

        m_ChildrenCount--;

        GenDebugString();

        return true;
    }

    int GetChildrenCount() const
    {
        return m_ChildrenCount;
    }

    void ResetReaminVaildGridCount()
    {
        m_RemainVaildGridCount = m_GridGroup->GetVaildGridCount();
    }

    int GetReaminVaildGridCount() const
    {
        return m_RemainVaildGridCount;
    }

    // 预先分配m_DebugString的空间，能在Debug下提升一些性能
    void RebuildDebugString()
    {
#if _DEBUG && DEBUG_STRING
        int size = GetHeight() * (GetWidth() * 3 + 1);
        if (m_DebugString != NULL)
            delete[] m_DebugString;
        m_DebugString = new char16_t[size + 1];
        m_DebugString[size] = '\0';

        int pos = 0;
        for (auto y = GetHeight() - 1; y >= 0; y--)
        {
            for (auto x = 0; x < GetWidth(); x++)
            {
                m_DebugString[pos++] = u' ';
                m_DebugString[pos++] = u' ';
                m_DebugString[pos++] = u' ';
            }
            m_DebugString[pos++] = u'\n';
        }
#endif
    }

    // 生成调试字符串，如下
    // ·, 表示这个位置没有格子，是无效位置
    // ×, 表示这个位置有格子，但是这个格子还没有放置东西
    // 0-9等符号, 表示这个位置有格子，且放置了东西，这个东西的id是0-9等符号
    // 注意：以下有多个1，表示1这个东西有多个，被多次放置了
    // 
    // ·  ·  ·  ×  ×  ·  ·  ·  ·  · 
    // 2  7  7  3  3  ·  ·  ·  ·  · 
    // 2  7  7  7  3  ·  ·  ·  ·  · 
    // 5  5  7  7  4  4  ·  ·  ·  · 
    // 5  5  5  7  4  4  ·  ·  ·  · 
    // 5  5  5  1  ·  ·  ·  ·  ·  · 
    // 9  9  9  9  ·  ·  ·  ·  ·  · 
    // 6  9  9  1  ·  3  3  ·  ·  · 
    // 6  9  9  8  8  8  3  ·  ·  · 
    // 6  9  9  1  8  8  ·  ·  ·  · 
    void GenDebugString()
    {
#if _DEBUG && DEBUG_STRING
        int pos = 0;
        for (auto y = GetHeight() - 1; y >= 0; y--)
        {
            for (auto x = 0; x < GetWidth(); x++)
            {
                pos++;
                auto grid = GetGridLocal(x, y);
                if (grid == NULL)
                    m_DebugString[pos++] = u'·';
                else if (grid->GetSubGrid() == NULL)
                    m_DebugString[pos++] = u'×';
                else
                {
                    auto owner = grid->GetSubGrid()->GetOwner();
                    if (owner != NULL && owner->GetId() >= 0)
                        m_DebugString[pos++] = u'0' + owner->GetId();
                    else
                        m_DebugString[pos++] = u'○';
                }
                pos++;
            }
            pos++;
        }
#endif
    }

private:
    GridGroup* m_GridGroup = NULL; // 原始数据
    int m_PosX = 0, m_PosY = 0; // 局部位置，左下角
    Rotation m_Rotation = Rotation::_0; // 旋转
    int m_ChildrenCount = 0;
    int m_RemainVaildGridCount = 0; // 剩余有效格子数量
    char16_t* m_DebugString; // 调试用字符串
};

class Item
{
public:
    Item(int classId)
    {
        m_ClassId = classId;
        m_GridGroup = new GridGroup();
        m_GridGroup->SetId(m_ClassId);
        m_GridGroupTransform = new GridGroupTransform(m_GridGroup);
    }

    ~Item()
    {
        if (m_GridGroup != NULL)
            delete m_GridGroup;
        m_GridGroup = NULL;

        if (m_GridGroupTransform != NULL)
            delete m_GridGroupTransform;
        m_GridGroupTransform = NULL;
    }

    int GetClassId() const
    {
        return m_ClassId;
    }

    GridGroup* GetGridGroup() const
    {
        return m_GridGroup;
    }

    GridGroupTransform* GetGridGroupTransform() const
    {
        return m_GridGroupTransform;
    }

private:
    int m_ClassId;
    GridGroup* m_GridGroup = NULL;
    GridGroupTransform* m_GridGroupTransform = NULL;
};

class Bag
{
public:
    Bag(int classId, int fixedPosX = -1, int fixedPosY = -1)
    {
        m_ClassId = classId;
        m_GridGroup = new GridGroup();
        m_GridGroup->SetId(m_ClassId);
        m_FixedPosX = fixedPosX;
        m_FixedPosY = fixedPosY;
        m_GridGroup->SetMoveable(m_FixedPosX < 0 || m_FixedPosY < 0);
        m_GridGroup->SetRotatable(m_FixedPosX < 0 || m_FixedPosY < 0);
        m_GridGroupTransform = new GridGroupTransform(m_GridGroup);
    }

    ~Bag()
    {
        if (m_GridGroup != NULL)
            delete m_GridGroup;
        m_GridGroup = NULL;

        if (m_GridGroupTransform != NULL)
            delete m_GridGroupTransform;
        m_GridGroupTransform = NULL;
    }

    int GetClassId() const
    {
        return m_ClassId;
    }

    GridGroup* GetGridGroup() const
    {
        return m_GridGroup;
    }

    GridGroupTransform* GetGridGroupTransform() const
    {
        return m_GridGroupTransform;
    }

    int GetFixedPosX() const
    {
        return m_FixedPosX;
    }

    int GetFixedPosY() const
    {
        return m_FixedPosY;
    }

private:
    int m_ClassId;
    GridGroup* m_GridGroup = NULL;
    GridGroupTransform* m_GridGroupTransform = NULL;
    // 左下角固定位置，如果两个都小于0，则可移动，可旋转
    int m_FixedPosX = -1, m_FixedPosY = -1;
};

class Backpack : public GridGroup
{
public:
    Backpack(int width, int height)
    {
        m_GridGroup = new GridGroup(width, height);
        for (auto x = 0; x < width; x++)
        {
            for (auto y = 0; y < height; y++)
            {
                m_GridGroup->SetGrid(x, y, new Grid());
            }
        }
    }

    ~Backpack()
    {
        if (m_GridGroup != NULL)
            delete m_GridGroup;
        m_GridGroup = NULL;
    }

    GridGroup* GetGridGroup() const
    {
        return m_GridGroup;
    }

private:
    GridGroup* m_GridGroup = NULL;
};

class BackpackPlacer
{
public:
    BackpackPlacer()
    {
    }

    ~BackpackPlacer()
    {
        if (m_Backpack != NULL)
            delete m_Backpack;
        m_Backpack = NULL;

        for (auto bag : m_Bags)
        {
            if (bag != NULL)
                delete bag;
        }

        for (auto item : m_Items)
        {
            if (item != NULL)
                delete item;
        }

        if (m_BackpackTrans != NULL)
            delete m_BackpackTrans;
        m_BackpackTrans = NULL;

        if (m_BackpackPlaceItemTrans != NULL)
            delete m_BackpackPlaceItemTrans;
        m_BackpackPlaceItemTrans = NULL;

        if (m_BackpackPlaceItemTransGridGroup != NULL)
            delete m_BackpackPlaceItemTransGridGroup;
        m_BackpackPlaceItemTransGridGroup = NULL;
    }

    void SetBackpack(Backpack* backpack)
    {
        if (m_Backpack != NULL)
            delete m_Backpack;
        m_Backpack = backpack;
    }

    void AddBag(Bag* bag)
    {
        m_Bags.push_back(bag);
        if (!bag->GetGridGroup()->GetMoveable())
        {
            // 不可移动包裹
            m_FixedBags.push_back(bag);
        }
        else
        {
            // 可移动包裹
            m_MovableBags.push_back(bag);

        }
    }

    void AddItem(Item* item)
    {
        m_Items.push_back(item);
    }

    void Place()
    {
        // 构造背包
        if (m_BackpackTrans != NULL)
            delete m_BackpackTrans;
        m_BackpackTrans = new GridGroupTransform(m_Backpack->GetGridGroup());

        if (m_BackpackPlaceItemTrans != NULL)
            delete m_BackpackPlaceItemTrans;
        if (m_BackpackPlaceItemTransGridGroup != NULL)
            delete m_BackpackPlaceItemTransGridGroup;
        m_BackpackPlaceItemTransGridGroup = new GridGroup(m_Backpack->GetGridGroup()->GetWidth(), m_Backpack->GetGridGroup()->GetHeight());
        m_BackpackPlaceItemTrans = new GridGroupTransform(m_BackpackPlaceItemTransGridGroup);



        // 优先放置不能移动的包裹
        for (auto bag : m_FixedBags)
        {
            auto trans = new GridGroupTransform(bag->GetGridGroup());
            trans->SetPos(bag->GetFixedPosX(), bag->GetFixedPosY());

            // 尝试放入包裹
            if (!m_BackpackTrans->TryAddTransform(trans))
            {
                // 这个位置不能放下，失败
                throw std::exception("固定包裹放置失败!");
            }
        }



        // 对物品大小由大到小排序
        std::sort(m_Items.begin(), m_Items.end(), [](Item* a, Item* b)
        {
            return a->GetGridGroup()->GetVaildGridCount() > b->GetGridGroup()->GetVaildGridCount();
        });
        for (int i = 0; i < (int)m_Items.size(); ++i)
            m_ItemSorts.push_back(i);



        // 对包裹大小由大到小排序
        std::sort(m_MovableBags.begin(), m_MovableBags.end(), [](Bag* a, Bag* b) 
        {
            return a->GetGridGroup()->GetVaildGridCount() > b->GetGridGroup()->GetVaildGridCount();
        });

        // 递归包裹放置顺序
        std::vector<int> bagSorts;
        for (int i = 0; i < (int)m_MovableBags.size(); ++i)
            bagSorts.push_back(i);

        for (int i = 0; i < (int)bagSorts.size(); i++)
        {
            int maxWidth = 1, maxHeight = 1; // 第一个包裹，固定在0,0位置，也不允许移动
            auto ret = PlaceBag(bagSorts, maxWidth, maxHeight, i);
            if (!ret)
                break;
        }

        // 使用std::next_permutation生成全排列
        //do
        //{
        //} while (std::next_permutation(bagSorts.begin(), bagSorts.end()));
    }

    // 返回是否继续
    bool PlaceBag(std::vector<int>& bagSort, int& maxWidth, int& maxHeight, int bagIdx)
    {
        if (bagIdx >= (int)bagSort.size())
        {
            // 所有包裹都放好了
            m_PlaceBagCount++;
            
            // 将格子布局放入新的位置
            auto gridGroup = m_Backpack->GetGridGroup();
            for (int x = 0; x < gridGroup->GetWidth(); x++)
            {
                for (int y = 0; y < gridGroup->GetHeight(); y++)
                {
                    auto grid = gridGroup->GetGrid(x, y);
                    m_BackpackPlaceItemTransGridGroup->SetGrid(x, y, NULL);
                    if (grid != NULL && grid->GetSubGrid() != NULL)
                        m_BackpackPlaceItemTransGridGroup->SetGrid(x, y, grid->GetSubGrid());
                }
            }
            m_BackpackPlaceItemTrans->SetPos(m_BackpackTrans->GetPosX(), m_BackpackTrans->GetPosY());
            m_BackpackPlaceItemTrans->SetRotate(m_BackpackTrans->GetRotate());
            m_BackpackPlaceItemTrans->ResetReaminVaildGridCount();

            // 放置物品
            for (int i = 0; i < (int)m_ItemSorts.size(); i++)
            {
                auto ret = PlaceItem(m_ItemSorts, i);
                if (!ret)
                    return false;
            }

            return true;
        }

        auto bag = m_MovableBags[bagSort[bagIdx]];
        auto bagTrans = bag->GetGridGroupTransform();
        // 遍历包裹能放入的所有位置，判断是否可以放入
        // 需要遍历的最大数值，是所有包裹的长宽之和
        for (int x = 0; x < maxWidth; x++)
        {
            for (int y = 0; y < maxHeight; y++)
            {
                // 设置位置
                bagTrans->SetPos(x, y);

                // 遍历所有旋转情况
                for (int r = 0; r < 4; r++)
                {
                    if (bagTrans->GetGridGroup()->GetRotationValid()[r])
                    {
                        // 设置旋转
                        bagTrans->SetRotate((Rotation)r);

                        // 必须不与本Transform重叠，且必须与本Transform相邻
                        if (!m_BackpackTrans->IsOverlapTransform(bagTrans) &&
                            (m_BackpackTrans->GetChildrenCount() == 0 || m_BackpackTrans->IsTouchTransform(bagTrans)))
                        {
                            // 尝试添加，在有上面两个判断下，这里不可能失败
                            if (!m_BackpackTrans->TryAddTransform(bagTrans))
                            {
                                throw std::exception("可移动包裹放置失败!");
                            }

                            maxWidth += bagTrans->GetWidth();
                            maxHeight += bagTrans->GetHeight();

                            // 继续放置下一个包裹, i可以等于bagSort.size()的原因是将成功条件全部归到本函数开头那段去
                            for (int i = bagIdx + 1; i <= (int)bagSort.size(); i++)
                            {
                                auto ret = PlaceBag(bagSort, maxWidth, maxHeight, i);
                                if (!ret)
                                    return false;
                            }

                            maxWidth -= bagTrans->GetWidth();
                            maxHeight -= bagTrans->GetHeight();

                            // 移除再继续
                            if (!m_BackpackTrans->TryRemoveTransform(bagTrans))
                            {
                                throw std::exception("可移动包裹移除失败!");
                            }
                        }
                    }
                }
            }
        }

        return true;
    }

    int GetPlaceBagCount() const
    {
        return m_PlaceBagCount;
    }

    bool PlaceItem(std::vector<int>& itemSort, int itemIdx)
    {
        if (m_BackpackPlaceItemTrans->GetReaminVaildGridCount() == 0)
        {
            // 背包已经放满
            m_PlaceItemCount++;
            return false;
        }
        else if (itemIdx >= (int)itemSort.size())
        {
            // 已将能放的全放入了
            return true;
        }

        auto item = m_Items[itemSort[itemIdx]];
        auto itemTrans = item->GetGridGroupTransform();
        if (item->GetGridGroup()->GetVaildGridCount() > m_BackpackPlaceItemTrans->GetReaminVaildGridCount())
        {
            // 物品过大
            return true;
        }

        // 遍历物品能放入的所有位置，判断是否可以放入
        for (int x = 0; x < m_BackpackPlaceItemTrans->GetWidth() - itemTrans->GetWidth() + 1; x++)
        {
            for (int y = 0; y < m_BackpackPlaceItemTrans->GetHeight() - itemTrans->GetHeight() + 1; y++)
            {
                // 设置位置
                itemTrans->SetPos(x, y);

                // 遍历所有旋转情况
                for (int r = 0; r < 4; r++)
                {
                    if (itemTrans->GetGridGroup()->GetRotationValid()[r])
                    {
                        // 设置旋转
                        itemTrans->SetRotate((Rotation)r);

                        // 必须不与本Transform重叠
                        if (!m_BackpackPlaceItemTrans->IsOverlapTransform(itemTrans))
                        {
                            // 尝试添加，在有上面两个判断下，这里不可能失败
                            if (!m_BackpackPlaceItemTrans->TryAddTransform(itemTrans))
                            {
                                throw std::exception("物品放置失败!");
                            }

                            // 继续放置下一个物品, i可以等于itemSort.size()的原因是将成功条件全部归到本函数开头那段去
                            for (int i = itemIdx + 1; i <= (int)itemSort.size(); i++)
                            {
                                auto ret = PlaceItem(itemSort, i);
                                if (!ret)
                                    return false;
                            }

                            // 移除再继续
                            if (!m_BackpackPlaceItemTrans->TryRemoveTransform(itemTrans))
                            {
                                throw std::exception("物品移除失败!");
                            }
                        }
                    }
                }
            }
        }

        return true;
    }

    int GetPlaceItemCount() const
    {
        return m_PlaceItemCount;
    }

private:
    Backpack* m_Backpack = NULL;
    std::vector<Bag*> m_Bags;
    std::vector<Bag*> m_MovableBags;
    std::vector<Bag*> m_FixedBags;
    std::vector<Item*> m_Items;

    GridGroupTransform* m_BackpackTrans = NULL;
    int m_PlaceBagCount = 0;

    GridGroupTransform* m_BackpackPlaceItemTrans = NULL;
    GridGroup* m_BackpackPlaceItemTransGridGroup = NULL;
    int m_PlaceItemCount = 0;

    std::vector<int> m_ItemSorts;
};

int main()
{
    BackpackPlacer backpackPlacer;

    auto backpack = new Backpack(10, 10);
    backpackPlacer.SetBackpack(backpack);
    int bagId = 1;

    {
        auto bag = new Bag(bagId++);
        bag->GetGridGroup()->AddGrid(0, 0, new Grid());
        bag->GetGridGroup()->AddGrid(0, 1, new Grid());
        bag->GetGridGroup()->AddGrid(1, 0, new Grid());
        bag->GetGridGroup()->AddGrid(1, 1, new Grid());
        backpackPlacer.AddBag(bag);
    }

    {
        auto bag = new Bag(bagId++);
        bag->GetGridGroup()->AddGrid(0, 0, new Grid());
        bag->GetGridGroup()->AddGrid(0, 1, new Grid());
        bag->GetGridGroup()->AddGrid(1, 0, new Grid());
        bag->GetGridGroup()->AddGrid(1, 1, new Grid());
        bag->GetGridGroup()->AddGrid(2, 1, new Grid());
        bag->GetGridGroup()->AddGrid(1, 2, new Grid());
        bag->GetGridGroup()->AddGrid(2, 2, new Grid());
        backpackPlacer.AddBag(bag);
    }

    {
        auto bag = new Bag(bagId++);
        bag->GetGridGroup()->AddGrid(0, 0, new Grid());
        bag->GetGridGroup()->AddGrid(0, 1, new Grid());
        bag->GetGridGroup()->AddGrid(1, 0, new Grid());
        bag->GetGridGroup()->AddGrid(1, 1, new Grid());
        bag->GetGridGroup()->AddGrid(2, 1, new Grid());
        bag->GetGridGroup()->AddGrid(1, 2, new Grid());
        bag->GetGridGroup()->AddGrid(2, 2, new Grid());
        backpackPlacer.AddBag(bag);
    }

    {
        auto bag = new Bag(bagId++);
        bag->GetGridGroup()->AddGrid(0, 0, new Grid());
        bag->GetGridGroup()->AddGrid(0, 1, new Grid());
        bag->GetGridGroup()->AddGrid(0, 2, new Grid());
        bag->GetGridGroup()->AddGrid(1, 0, new Grid());
        bag->GetGridGroup()->AddGrid(1, 1, new Grid());
        bag->GetGridGroup()->AddGrid(1, 2, new Grid());
        bag->GetGridGroup()->AddGrid(2, 0, new Grid());
        bag->GetGridGroup()->AddGrid(2, 1, new Grid());
        bag->GetGridGroup()->AddGrid(2, 2, new Grid());
        backpackPlacer.AddBag(bag);
    }

    {
        auto bag = new Bag(bagId++);
        bag->GetGridGroup()->AddGrid(0, 0, new Grid());
        bag->GetGridGroup()->AddGrid(0, 1, new Grid());
        bag->GetGridGroup()->AddGrid(0, 2, new Grid());
        bag->GetGridGroup()->AddGrid(1, 0, new Grid());
        bag->GetGridGroup()->AddGrid(1, 1, new Grid());
        bag->GetGridGroup()->AddGrid(1, 2, new Grid());
        bag->GetGridGroup()->AddGrid(2, 0, new Grid());
        bag->GetGridGroup()->AddGrid(2, 1, new Grid());
        bag->GetGridGroup()->AddGrid(2, 2, new Grid());
        bag->GetGridGroup()->AddGrid(3, 0, new Grid());
        bag->GetGridGroup()->AddGrid(3, 1, new Grid());
        bag->GetGridGroup()->AddGrid(3, 2, new Grid());
        backpackPlacer.AddBag(bag);
    }

    {
        auto bag = new Bag(bagId++);
        bag->GetGridGroup()->AddGrid(0, 0, new Grid());
        bag->GetGridGroup()->AddGrid(0, 1, new Grid());
        bag->GetGridGroup()->AddGrid(0, 2, new Grid());
        bag->GetGridGroup()->AddGrid(1, 0, new Grid());
        bag->GetGridGroup()->AddGrid(1, 1, new Grid());
        bag->GetGridGroup()->AddGrid(1, 2, new Grid());
        bag->GetGridGroup()->AddGrid(2, 0, new Grid());
        bag->GetGridGroup()->AddGrid(2, 1, new Grid());
        bag->GetGridGroup()->AddGrid(2, 2, new Grid());
        bag->GetGridGroup()->AddGrid(3, 0, new Grid());
        bag->GetGridGroup()->AddGrid(3, 1, new Grid());
        bag->GetGridGroup()->AddGrid(3, 2, new Grid());
        backpackPlacer.AddBag(bag);
    }

    int itemId = 1;

    for (int i = 0; i < 20; i++)
    {
        auto item = new Item(itemId);
        item->GetGridGroup()->AddGrid(0, 0, new Grid());
        backpackPlacer.AddItem(item);
    }
    itemId++;

    for (int i = 0; i < 4; i++)
    {
        auto item = new Item(itemId);
        item->GetGridGroup()->AddGrid(0, 0, new Grid());
        item->GetGridGroup()->AddGrid(0, 1, new Grid());
        backpackPlacer.AddItem(item);
    }
    itemId++;

    for (int i = 0; i < 3; i++)
    {
        auto item = new Item(itemId);
        item->GetGridGroup()->AddGrid(0, 0, new Grid());
        item->GetGridGroup()->AddGrid(0, 1, new Grid());
        item->GetGridGroup()->AddGrid(1, 0, new Grid());
        backpackPlacer.AddItem(item);
    }
    itemId++;

    for (int i = 0; i < 2; i++)
    {
        auto item = new Item(itemId);
        item->GetGridGroup()->AddGrid(0, 0, new Grid());
        item->GetGridGroup()->AddGrid(0, 1, new Grid());
        item->GetGridGroup()->AddGrid(1, 0, new Grid());
        item->GetGridGroup()->AddGrid(1, 1, new Grid());
        backpackPlacer.AddItem(item);
    }
    itemId++;

    {
        auto item = new Item(itemId);
        item->GetGridGroup()->AddGrid(0, 0, new Grid());
        item->GetGridGroup()->AddGrid(0, 1, new Grid());
        item->GetGridGroup()->AddGrid(0, 2, new Grid());
        item->GetGridGroup()->AddGrid(1, 0, new Grid());
        item->GetGridGroup()->AddGrid(1, 1, new Grid());
        item->GetGridGroup()->AddGrid(1, 2, new Grid());
        item->GetGridGroup()->AddGrid(2, 0, new Grid());
        item->GetGridGroup()->AddGrid(2, 1, new Grid());
        backpackPlacer.AddItem(item);
    }
    itemId++;

    {
        auto item = new Item(itemId);
        item->GetGridGroup()->AddGrid(0, 0, new Grid());
        item->GetGridGroup()->AddGrid(1, 0, new Grid());
        item->GetGridGroup()->AddGrid(2, 0, new Grid());
        backpackPlacer.AddItem(item);
    }
    itemId++;

    {
        auto item = new Item(itemId);
        item->GetGridGroup()->AddGrid(0, 0, new Grid());
        item->GetGridGroup()->AddGrid(0, 1, new Grid());
        item->GetGridGroup()->AddGrid(1, 0, new Grid());
        item->GetGridGroup()->AddGrid(1, 1, new Grid());
        item->GetGridGroup()->AddGrid(2, 1, new Grid());
        item->GetGridGroup()->AddGrid(1, 2, new Grid());
        item->GetGridGroup()->AddGrid(2, 2, new Grid());
        item->GetGridGroup()->AddGrid(3, 2, new Grid());
        backpackPlacer.AddItem(item);
    }
    itemId++;

    {
        auto item = new Item(itemId);
        item->GetGridGroup()->AddGrid(0, 0, new Grid());
        item->GetGridGroup()->AddGrid(0, 1, new Grid());
        item->GetGridGroup()->AddGrid(1, 0, new Grid());
        item->GetGridGroup()->AddGrid(1, 1, new Grid());
        item->GetGridGroup()->AddGrid(1, 2, new Grid());
        backpackPlacer.AddItem(item);
    }
    itemId++;

    {
        auto item = new Item(itemId);
        item->GetGridGroup()->AddGrid(0, 0, new Grid());
        item->GetGridGroup()->AddGrid(0, 1, new Grid());
        item->GetGridGroup()->AddGrid(0, 2, new Grid());
        item->GetGridGroup()->AddGrid(1, 2, new Grid());
        backpackPlacer.AddItem(item);
    }
    itemId++;

    {
        auto item = new Item(itemId);
        item->GetGridGroup()->AddGrid(0, 0, new Grid());
        item->GetGridGroup()->AddGrid(1, 0, new Grid());
        item->GetGridGroup()->AddGrid(2, 0, new Grid());
        item->GetGridGroup()->AddGrid(2, 1, new Grid());
        backpackPlacer.AddItem(item);
    }
    itemId++;

    {
        auto item = new Item(itemId);
        item->GetGridGroup()->AddGrid(0, 0, new Grid());
        item->GetGridGroup()->AddGrid(1, 0, new Grid());
        item->GetGridGroup()->AddGrid(2, 0, new Grid());
        backpackPlacer.AddItem(item);
    }
    itemId++;

    {
        auto item = new Item(itemId);
        item->GetGridGroup()->AddGrid(1, 0, new Grid());
        item->GetGridGroup()->AddGrid(2, 0, new Grid());
        item->GetGridGroup()->AddGrid(1, 1, new Grid());
        item->GetGridGroup()->AddGrid(2, 1, new Grid());
        item->GetGridGroup()->AddGrid(1, 2, new Grid());
        item->GetGridGroup()->AddGrid(2, 2, new Grid());
        item->GetGridGroup()->AddGrid(0, 3, new Grid());
        item->GetGridGroup()->AddGrid(1, 3, new Grid());
        item->GetGridGroup()->AddGrid(2, 3, new Grid());
        item->GetGridGroup()->AddGrid(3, 3, new Grid());
        backpackPlacer.AddItem(item);
    }
    itemId++;

    {
        auto item = new Item(itemId);
        backpackPlacer.AddItem(item);
    }
    itemId++;

    // 开始时间，微秒
    auto startUs = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now()).time_since_epoch().count();

    // 放置
    backpackPlacer.Place();

    // 结束时间，微秒
    auto endUs = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now()).time_since_epoch().count();

    // 打印结果
    std::cout << "耗费时间: " << (endUs - startUs) * 0.001f << "ms, 包裹成功放置次数: " << backpackPlacer.GetPlaceBagCount() << ", 物品成功放置次数: " << backpackPlacer.GetPlaceItemCount() << std::endl;
}
